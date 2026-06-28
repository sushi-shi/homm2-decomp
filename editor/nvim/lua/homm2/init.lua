-- homm2.nvim - in-editor objdiff for the homm2 binary-matching project.
--
-- For the function under the cursor: its target (retail) asm, its base
-- (recompiled) asm, a side-by-side diff, and its match %. Plus a status
-- overview and a one-key build that reports what moved.
--
-- Presentation only, no bespoke tool: every asm/diff view IS one
-- `objdiff-cli diff ... --format json` invocation, and the cursor->function
-- resolution reads the data the pipeline already emits -
-- build/gen/symbol_names.csv (addr -> mangled symbol, unit) and
-- build/objdiff/report.json (per-function match %). The retail ADDRESS in each
-- function's VA(...) macro is the join key - it moves with the function text,
-- so nothing here goes stale the way line tables would.
--
-- Source overlay (config.source_lines, toggle `vL` / `:Homm2 source`): on the
-- BASE side only (vb, and the base pane of vd): each source STATEMENT is floated as
-- a virtual line above the asm it lowered to, and each `[ebp±off]` access is tagged
-- end-of-line with its local/param NAME (`mov eax, [ebp-0x10] ; cell`). Data:
-- build/lines/<unit>.json from scripts/gen_lines.py (compiles the TU /Z7 -
-- codegen-neutral - and reads its COFF line table + .debug$S S_BPREL32 locals; needs
-- the `nix develop .#build` shell for wine cl). Retail has no debug info, so
-- target/diff-target panes are never annotated.

local M = {}
local uv = vim.uv or vim.loop

M.config = {
  keymaps = true,             -- vt/vb (asm), vd (diff), vs (status), vB (build), V (peek)
  hints = true,               -- inline match-% virtual text after each VA(...) line
  source_lines = true,        -- overlay each source statement above its asm (BASE side only: vb + vd base pane)
  build_on_save = false,      -- rebuild (quietly) whenever a TU is saved
  format_on_save = false,     -- clang-format the saved file in place (root .clang-format)
  split = "botright vsplit",  -- where asm/status views open
}

-- Repo-relative paths the plugin reads / drives.
local CSV     = "build/gen/symbol_names.csv"
local REPORT  = "build/objdiff/report.json"
local ODIR    = "build/objdiff"           -- objdiff project dir + base objs (base/<unit>.obj)
local DELINK  = "build/delink"            -- target (retail-derived) objs: <unit>.c.obj
local ROOT_MARKER = "config/units.toml" -- tracked: present even in a fresh checkout

-- The VA(...) macros in src/ hold a VA (image-base-included, e.g. 0x0040b070),
-- but symbol_names.csv's rva column is image-base-RELATIVE (0x000b070). Bridge
-- the two so the cursor->function join works regardless of which a lookup uses.
-- HEROES2W.EXE image base = 0x400000 (relative and VA key-spaces don't overlap).
local IMGBASE = 0x400000

-- ------------------------------------------------------------------ logging --
-- Every objdiff/build invocation is logged (:Homm2Log) - a stale dev shell
-- shipping an old objdiff-cli is the classic silent killer.
M._log = {}
local function log(line)
  table.insert(M._log, os.date("%H:%M:%S ") .. line)
  if #M._log > 200 then table.remove(M._log, 1) end
end

local function notify(msg, level)
  vim.notify("homm2: " .. msg, level or vim.log.levels.INFO)
end

-- ------------------------------------------------------------------- project --

--- Walk up from the buffer's file to the checkout root (the dir with
--- config/units.toml). Sibling worktrees each resolve to their own build/.
local function project_root(bufnr)
  local file = vim.api.nvim_buf_get_name(bufnr or 0)
  if file == "" then return nil end
  for dir in vim.fs.parents(file) do
    if uv.fs_stat(dir .. "/" .. ROOT_MARKER) then return dir end
  end
end

-- --------------------------------------------------------------- database io --
-- All reads are cached against the file's mtime: cheap, and a build bumps the
-- mtime so the next query reads fresh automatically.

local sym_cache = {}    -- root -> {mtime, by_addr, by_unit}
local report_cache = {} -- root -> {mtime, units, overall}

local function parse_addr(s) return s and tonumber(s) or nil end

--- symbol_names.csv -> {by_addr = {int -> {name,unit,size,kind}},
---                      by_unit = {unit -> {entries...}}}.
--- Mangled names never contain commas, so a plain comma split is safe.
local function load_symbols(root)
  local path = root .. "/" .. CSV
  local st = uv.fs_stat(path)
  if not st then return nil end
  local c = sym_cache[root]
  if c and c.mtime == st.mtime.sec then return c end
  c = { mtime = st.mtime.sec, by_addr = {}, by_unit = {} }
  local first = true
  for line in io.lines(path) do
    if first then first = false                    -- skip the header row
    else
      local rva, name, unit, size, kind =
        line:match("^([^,]*),([^,]*),([^,]*),([^,]*),([^,]*)$")
      local addr = parse_addr(rva)
      if addr and name ~= "" then
        local e = { name = name, unit = unit, size = size, kind = kind, addr = rva }
        c.by_addr[addr] = e                  -- by relative RVA (the CSV column)
        c.by_addr[addr + IMGBASE] = e        -- and by VA, since the VA(...) macros hold a VA
        c.by_unit[unit] = c.by_unit[unit] or {}
        table.insert(c.by_unit[unit], e)
      end
    end
  end
  sym_cache[root] = c
  return c
end

--- report.json -> {units = {name -> {pct, funcs = {fn -> pct}}}, overall}.
local function read_report(root)
  local path = root .. "/" .. REPORT
  local fd = io.open(path, "r")
  if not fd then return nil end
  local ok, data = pcall(vim.json.decode, fd:read("*a"))
  fd:close()
  if not ok then return nil end
  local out = { units = {}, order = {},
                overall = (data.measures or {}).fuzzy_match_percent or 0 }
  for _, u in ipairs(data.units or {}) do
    local m = u.measures or {}
    local rec = { pct = m.fuzzy_match_percent or 0,
                  mf = tonumber(m.matched_functions) or 0,
                  tf = tonumber(m.total_functions) or 0,
                  funcs = {} }
    for _, f in ipairs(u.functions or {}) do
      rec.funcs[f.name] = f.fuzzy_match_percent or 0
    end
    out.units[u.name] = rec
    table.insert(out.order, u.name)
  end
  return out
end

local function load_report(root)
  local path = root .. "/" .. REPORT
  local st = uv.fs_stat(path)
  if not st then return nil end
  local c = report_cache[root]
  if c and c.mtime == st.mtime.sec then return c.rep end
  local rep = read_report(root)
  if rep then report_cache[root] = { mtime = st.mtime.sec, rep = rep } end
  return rep
end

local function invalidate(root)
  sym_cache[root], report_cache[root] = nil, nil
end

-- ----------------------------------------------------- cursor -> function ---

--- {name, unit, addr} for the function the cursor sits in, or nil + reason.
--- An exact 0x.. function address under the cursor wins; otherwise the nearest
--- VA(0x..)/VAU(0x..) at or above the cursor selects the function.
local function symbol_at(root, buf, lnum)
  local db = load_symbols(root)
  if not db then return nil, "no " .. CSV .. " - run :Homm2Build first" end

  local cword = vim.fn.expand("<cword>")
  local under = cword:match("^0[xX]%x+$")
  if under then
    local e = db.by_addr[tonumber(under)]
    if e then return { name = e.name, unit = e.unit, addr = e.addr } end
  end

  local lines = vim.api.nvim_buf_get_lines(buf, 0, lnum, false)
  for i = #lines, 1, -1 do
    local a = lines[i]:match("VAU?%s*%(%s*(0[xX]%x+)")
    if a then
      local e = db.by_addr[tonumber(a)]
      if e then return { name = e.name, unit = e.unit, addr = a } end
      return nil, ("VA " .. a .. " is not in " .. CSV .. " - rebuild?")
    end
  end
  return nil, "no VA(...) above the cursor - this region isn't matched yet "
    .. "(try :Homm2 status)"
end

-- --------------------------------------------------------- objdiff backend ---

local diff_cache = {} -- key -> decoded unit diff json

-- Save-time per-unit match %s from `objdiff-cli diff -u` (root -> unit -> {name -> pct}).
-- The fast build-on-save path recompiles ONLY the edited unit and diffs it against
-- the cached target objs, so it never regenerates the all-units report.json. These
-- live %s overlay the (now staler) report.json for the edited unit's hints/peek; a
-- full `:Homm2Build` refreshes report.json and clears the overlay.
local live_pct = {}

--- The whole unit's diff json (objdiff one-shot returns the unit, not just one
--- symbol). Cached against the two objs' mtimes. `cb(json)` on success.
local function unit_diff(root, unit, focus, cb)
  if vim.fn.executable("objdiff-cli") == 0 then
    return notify("objdiff-cli not on PATH - launch nvim from `nix develop`",
      vim.log.levels.ERROR)
  end
  local t = uv.fs_stat(root .. "/" .. DELINK .. "/" .. unit .. ".c.obj")
  local b = uv.fs_stat(root .. "/" .. ODIR .. "/base/" .. unit .. ".obj")
  local key = table.concat({ root, unit,
    t and t.mtime.sec or 0, b and b.mtime.sec or 0 }, "|")
  if diff_cache[key] then return cb(diff_cache[key]) end

  local cmd = { "objdiff-cli", "diff", "-p", ODIR, "-u", unit,
                "-o", "-", "--format", "json" }
  if focus and focus ~= "" then cmd[#cmd + 1] = focus end  -- focus is optional: omit -> whole unit
  log(table.concat({ "objdiff-cli diff -u", unit, focus or "" }, " ") .. "  [" .. root .. "]")
  vim.system(cmd, { cwd = root, text = true }, function(res)
    vim.schedule(function()
      if res.code ~= 0 then
        return notify("objdiff-cli failed: " ..
          (res.stderr or ""):gsub("%s+$", ""), vim.log.levels.ERROR)
      end
      local ok, json = pcall(vim.json.decode, res.stdout or "")
      if not ok or type(json) ~= "table" then
        return notify("objdiff-cli: unparseable output", vim.log.levels.ERROR)
      end
      diff_cache[key] = json
      cb(json)
    end)
  end)
end

--- The symbol record (with instructions + match_percent) for `name` on `side`
--- ("left" = target, "right" = base).
local function pick(json, side, name)
  for _, sym in ipairs((json[side] or {}).symbols or {}) do
    if sym.name == name then return sym end
  end
end

--- {mangled_name -> match_percent} for every diffed symbol in a unit-diff json.
--- left (target) is canonical, so it wins over right (base) for a shared name.
local function pcts_from_diff(json)
  local out = {}
  for _, side in ipairs({ "right", "left" }) do
    for _, s in ipairs((json[side] or {}).symbols or {}) do
      if s.name and s.match_percent ~= nil then out[s.name] = s.match_percent end
    end
  end
  return out
end

--- objdiff instructions -> decorated asm lines. Intra-function jump targets get
--- synthetic labels (`.L1:` ...) and branches reference them (`jne short .L1`)
--- instead of raw offsets - more readable, AND the diff stays clean since a
--- label name matches on both sides even when the byte offset shifts. Absolute
--- offsets are otherwise dropped for the same reason. Calls/external refs keep
--- objdiff's symbol formatting untouched.
local function asm_lines(sym)
  local ins = sym.instructions or {}

  local is_addr = {}
  for _, i in ipairs(ins) do is_addr[tonumber((i.instruction or {}).address) or 0] = true end

  local dests = {}  -- branch targets that land on an instruction in THIS function
  for _, i in ipairs(ins) do
    local d = tonumber((i.instruction or {}).branch_dest or "")
    if d and is_addr[d] then dests[d] = true end
  end
  local ordered = {}
  for d in pairs(dests) do ordered[#ordered + 1] = d end
  table.sort(ordered)
  local label = {}
  for idx, d in ipairs(ordered) do label[d] = ".L" .. idx end

  -- `out` = display lines; for each instruction row, `addrs[i]` = its code offset
  -- and `raws[i]` = its index in objdiff's instruction array (label rows: both nil).
  -- objdiff's left/right arrays are index-PAIRED, so raws is the join that aligns a
  -- base row with its target row across the two panes (their addresses differ).
  local out, addrs, raws = {}, {}, {}
  for k, i in ipairs(ins) do
    local I = i.instruction or {}
    -- Skip objdiff's diff-alignment GAPS: empty placeholder rows (no `formatted`,
    -- diff_kind INSERT/DELETE) it inserts so the two sides line up. They are not
    -- real instructions on this side; rendering them as "?" is noise, and the
    -- side-by-side diff lets Neovim do its own alignment.
    if I.formatted then
      local off = tonumber(I.address) or 0
      if label[off] then out[#out + 1] = label[off] .. ":" end
      local text = I.formatted
      local d = tonumber(I.branch_dest or "")
      if d and label[d] then text = text:gsub(("0x%x"):format(d), label[d]) end
      out[#out + 1] = "    " .. text
      addrs[#out] = off
      raws[#out] = k
    end
  end
  if #out == 0 then out = { "(no instructions)" } end
  return out, addrs, raws
end

local function pct(x) return string.format("%.2f%%", x or 0) end

-- --------------------------------------------------- source-line overlay ---
-- BASE-only (our /Z7 build carries line info; retail has none). For vb and the
-- base pane of vd we float each source STATEMENT above the asm it lowered to, as
-- transient virtual lines (the buffer text is unchanged). Data: build/lines/
-- <unit>.json from scripts/gen_lines.py (compiles the TU /Z7 - codegen-neutral, so
-- the COFF line-table offsets equal objdiff's base-side instruction addresses).
local SRC_NS = vim.api.nvim_create_namespace("homm2_source")
local source_cache = {}    -- "root|unit" -> { mtime, map = {mangled -> {offset -> "Lnnn  text"}} }
local source_pending = {}  -- "root|unit" -> true while gen_lines runs (no double-spawn)

local function read_source_json(root, unit)
  local fd = io.open(root .. "/build/lines/" .. unit .. ".json", "r")
  if not fd then return nil end
  local data = fd:read("*a"); fd:close()
  local ok, j = pcall(vim.json.decode, data)
  if not ok or type(j) ~= "table" then return nil end
  local stmts, locals = {}, {}
  for mangled, rows in pairs(j.stmts or {}) do
    local m = {}
    for _, r in ipairs(rows) do m[r[1]] = ("%d: %s"):format(r[2], r[3]) end  -- {offset -> "line: src"}
    stmts[mangled] = m
  end
  for mangled, rows in pairs(j.locals or {}) do
    local m = {}
    for _, r in ipairs(rows) do m[r[1]] = r[2] end                          -- {bp_offset -> name}
    locals[mangled] = m
  end
  return { stmts = stmts, locals = locals }
end

--- {mangled -> {offset -> label}} for a unit; cached by the base obj's mtime,
--- (re)generated lazily. Needs the build shell (wine cl); silent no-op otherwise.
local function source_map(root, unit, cb)
  local st = uv.fs_stat(root .. "/" .. ODIR .. "/base/" .. unit .. ".obj")
  local mt = st and st.mtime.sec or 0
  local key = root .. "|" .. unit
  local c = source_cache[key]
  if c and c.mtime == mt and c.map then return cb(c.map) end
  local js = uv.fs_stat(root .. "/build/lines/" .. unit .. ".json")
  if js and js.mtime.sec >= mt then
    local map = read_source_json(root, unit)
    if map then source_cache[key] = { mtime = mt, map = map }; return cb(map) end
  end
  if source_pending[key] then return end
  source_pending[key] = true
  log("gen_lines " .. unit .. "  [" .. root .. "]")
  vim.system({ "python3", "scripts/gen_lines.py", unit }, { cwd = root, text = true },
    function(res)
      vim.schedule(function()
        source_pending[key] = nil
        if res.code ~= 0 then
          return log("gen_lines failed: " .. ((res.stderr or ""):gsub("%s+$", "")))
        end
        local map = read_source_json(root, unit)
        if map then source_cache[key] = { mtime = mt, map = map }; cb(map) end
      end)
    end)
end

--- Place / refresh the source-statement overlay. Driven by the BASE buffer only
--- (single vb, or the base pane of a diff). The base buffer shows each statement's
--- text above its asm; in a diff, the SAME virtual line is mirrored as an empty `;`
--- on the TARGET pane at the index-PAIRED row (ctx.sibling_target + ctx.traws), so
--- the two scrollbound panes grow by equal virtual lines and stay aligned - a
--- base-only overlay would shift base down out of sync. Target/vt are never driven
--- directly. Clears first (its buffer + the sibling) so toggle-off cleans up both.
--- Caveat: a statement whose first instruction is a base-only insertion (the target
--- has a GAP there) has no target row to pad, so it drifts the panes by one below
--- that point; perfectly matched functions have no gaps and stay exact.
local function attach_source_lines(buf, ctx)
  if not (ctx and ctx.side == "base" and vim.api.nvim_buf_is_valid(buf)) then return end
  pcall(vim.api.nvim_buf_clear_namespace, buf, SRC_NS, 0, -1)
  if ctx.sibling_target and vim.api.nvim_buf_is_valid(ctx.sibling_target) then
    pcall(vim.api.nvim_buf_clear_namespace, ctx.sibling_target, SRC_NS, 0, -1)
  end
  if not (M.config.source_lines and ctx.addrs and ctx.name and ctx.unit and ctx.root) then return end
  source_map(ctx.root, ctx.unit, function(map)
    if not (map and vim.api.nvim_buf_is_valid(buf)) then return end
    local stmts = map.stmts[ctx.name]
    local locals = map.locals[ctx.name]
    if not (stmts or locals) then return end
    pcall(vim.api.nvim_buf_clear_namespace, buf, SRC_NS, 0, -1)
    local off0 = ctx.line_offset or 0
    -- (1) statement text above each statement's first instruction (+ aligned target pad)
    if stmts then
      local tbuf, inv_t = ctx.sibling_target, nil   -- target row by raw index, for padding
      if tbuf and ctx.traws and vim.api.nvim_buf_is_valid(tbuf) then
        pcall(vim.api.nvim_buf_clear_namespace, tbuf, SRC_NS, 0, -1)
        inv_t = {}; for ti, k in pairs(ctx.traws) do inv_t[k] = ti end
      end
      for i, addr in pairs(ctx.addrs) do
        if stmts[addr] then
          pcall(vim.api.nvim_buf_set_extmark, buf, SRC_NS, (i - 1) + off0, 0,
            { virt_lines = { { { stmts[addr], "Comment" } } }, virt_lines_above = true })
          local ti = inv_t and ctx.raws and inv_t[ctx.raws[i]]   -- paired target row
          if ti then
            pcall(vim.api.nvim_buf_set_extmark, tbuf, SRC_NS, ti - 1, 0,
              { virt_lines = { { { ";", "Comment" } } }, virt_lines_above = true })
          end
        end
      end
    end
    -- (2) local/param name after each `[ebp±off]` access (end-of-line; adds no rows,
    -- so it never disturbs the diff alignment).
    if locals then
      for row, line in ipairs(vim.api.nvim_buf_get_lines(buf, 0, -1, false)) do
        local sign, hex = line:match("%[ebp([%-%+])(0x%x+)%]")
        if sign then
          local nm = locals[tonumber(hex) * (sign == "-" and -1 or 1)]
          if nm then
            pcall(vim.api.nvim_buf_set_extmark, buf, SRC_NS, row - 1, 0,
              { virt_text = { { " ; " .. nm, "Comment" } }, virt_text_pos = "eol" })
          end
        end
      end
    end
  end)
end

--- Re-apply (or clear, when toggled off) the overlay on every open view; the base
--- pane drives its target sibling, so both diff panes stay aligned.
local function refresh_all_source()
  for _, b in ipairs(vim.api.nvim_list_bufs()) do
    if vim.api.nvim_buf_is_valid(b) and vim.b[b].homm2 then
      attach_source_lines(b, vim.b[b].homm2)
    end
  end
end

-- --------------------------------------------------------------- rendering ---

local function fill(buf, lines, ctx)
  vim.bo[buf].modifiable = true
  vim.api.nvim_buf_set_lines(buf, 0, -1, false, lines)
  vim.bo[buf].modifiable = false
  vim.bo[buf].buftype = "nofile"
  vim.bo[buf].swapfile = false
  vim.bo[buf].bufhidden = ctx.transient and "wipe" or "hide"
  vim.bo[buf].filetype = ctx.filetype or "asm"
  vim.b[buf].homm2 = ctx
  local opts = { buffer = buf, nowait = true, silent = true }
  vim.keymap.set("n", "q", "<cmd>close<cr>", opts)
  vim.keymap.set("n", "<CR>", function() M.follow() end, opts)
  vim.keymap.set("n", "V", function() M.follow() end, opts)
  pcall(attach_source_lines, buf, ctx)   -- BASE asm only; no-op elsewhere
end

-- Replace a scratch view's content in place (re-render after a build). The view
-- state (cursor, scroll) of every window showing it is preserved, so a refresh
-- doesn't move you - winrestview clamps the cursor if the line count shrank.
local function set_buf_lines(buf, lines)
  local views = {}
  for _, w in ipairs(vim.fn.win_findbuf(buf)) do
    views[w] = vim.api.nvim_win_call(w, vim.fn.winsaveview)
  end
  vim.bo[buf].modifiable = true
  vim.api.nvim_buf_set_lines(buf, 0, -1, false, lines)
  vim.bo[buf].modifiable = false
  for w, v in pairs(views) do
    if vim.api.nvim_win_is_valid(w) then
      vim.api.nvim_win_call(w, function() vim.fn.winrestview(v) end)
    end
  end
end

--- One reusable scratch buffer per (kind, symbol): rerunning the same query
--- refreshes it in place; different queries coexist.
local function show_split(lines, ctx)
  local tag = (ctx.tag or "view"):gsub("[^%w_:~.$?@/]+", "."):sub(1, 90)
  local name = "homm2://" .. tag
  local buf = vim.fn.bufnr("^" .. vim.fn.fnameescape(name) .. "$")
  if buf == -1 then
    buf = vim.api.nvim_create_buf(true, true)
    vim.api.nvim_buf_set_name(buf, name)
  end
  fill(buf, lines, ctx)
  local win
  for _, w in ipairs(vim.api.nvim_tabpage_list_wins(0)) do
    if vim.api.nvim_win_get_buf(w) == buf then win = w break end
  end
  if win then vim.api.nvim_set_current_win(win)
  else vim.cmd(M.config.split); vim.api.nvim_win_set_buf(0, buf) end
  if ctx.cursor then pcall(vim.api.nvim_win_set_cursor, 0, { ctx.cursor, 0 }) end
end

--- A non-focusable float. Two flavours:
---   * default ("cursor"): a hover at the cursor that the next cursor move /
---     buffer leave dismisses (used by V peek).
---   * ctx.pos == "editor": a corner popup that fades after ctx.timeout ms and
---     never steals focus (used by the build-complete message, which may land
---     while you are typing elsewhere).
local function show_float(lines, ctx)
  local width = 0
  for _, l in ipairs(lines) do width = math.max(width, #l, ctx.title and #ctx.title or 0) end
  width = math.min(width + 1, vim.o.columns - 4)
  local height = math.min(#lines, math.floor(vim.o.lines * 0.6))
  ctx.transient = true
  local src_buf = vim.api.nvim_get_current_buf()
  local buf = vim.api.nvim_create_buf(false, true)
  fill(buf, lines, ctx)
  local cfg = { width = width, height = height, style = "minimal",
    border = "single", focusable = false, noautocmd = true,
    title = ctx.title, title_pos = "left" }
  if ctx.pos == "editor" then
    cfg.relative, cfg.anchor, cfg.row, cfg.col = "editor", "NE", 1, vim.o.columns - 2
  else
    cfg.relative, cfg.row, cfg.col = "cursor", 1, 0
  end
  local win = vim.api.nvim_open_win(buf, false, cfg)  -- enter=false: keep focus
  local function close()
    if vim.api.nvim_win_is_valid(win) then vim.api.nvim_win_close(win, true) end
  end
  if ctx.timeout then
    vim.defer_fn(close, ctx.timeout)
  else
    -- Deferred so the float's own layout change doesn't trip the dismiss at once.
    vim.schedule(function()
      vim.api.nvim_create_autocmd(
        { "CursorMoved", "CursorMovedI", "InsertEnter", "BufLeave", "WinLeave" },
        { buffer = src_buf, once = true, callback = close })
    end)
  end
end

--- A small persistent corner note (e.g. "building ..."); returns a closer.
local function show_note(text)
  local buf = vim.api.nvim_create_buf(false, true)
  vim.bo[buf].bufhidden = "wipe"
  vim.api.nvim_buf_set_lines(buf, 0, -1, false, { " " .. text .. " " })
  local win = vim.api.nvim_open_win(buf, false, {
    relative = "editor", anchor = "NE", row = 1, col = vim.o.columns - 2,
    width = #text + 2, height = 1, style = "minimal", border = "single",
    focusable = false, noautocmd = true,
  })
  return function()
    if vim.api.nvim_win_is_valid(win) then vim.api.nvim_win_close(win, true) end
  end
end

-- Label each diff pane via its winbar - which side, the symbol, its %. winbar
-- is window chrome (not buffer text), so it labels the pane unambiguously and
-- can never show up as a spurious diff line. `%` is escaped because winbar uses
-- statusline format syntax.
local function set_winbar(win, label, name, p)
  local s = ("%s   %s   %.2f%%"):format(label, name, p or 0)
  vim.wo[win].winbar = (s:gsub("%%", "%%%%"))
end

--- target (left) | base (right) as a native nvim diff: scrollbound, changes
--- highlighted - the objdiff look, in the editor. A winbar labels each pane
--- TARGET (retail) / BASE (recompiled) so the sides are never ambiguous.
local function show_diff(root, sym, t, b)
  local function pane(side, s, extra)
    local name = "homm2://diff." .. side .. "/" .. sym.name:gsub("[^%w_:~.$?@]+", ".")
    local buf = vim.fn.bufnr("^" .. vim.fn.fnameescape(name) .. "$")
    if buf == -1 then
      buf = vim.api.nvim_create_buf(true, true)
      vim.api.nvim_buf_set_name(buf, name)
    end
    local lines, addrs, raws = asm_lines(s)
    local ctx = { root = root, filetype = "asm", kind = "diffpane",
                  name = sym.name, unit = sym.unit, side = side,
                  addrs = addrs, raws = raws, line_offset = 0 }
    for k, v in pairs(extra or {}) do ctx[k] = v end
    fill(buf, lines, ctx)
    return buf, raws
  end

  -- Lighter than nvim's default diff (which washes the whole CHANGED line via
  -- DiffChange): drop that line wash and keep only the changed TOKEN, underlined
  -- (DiffText). Added/removed lines still show (DiffAdd/DiffDelete). Scoped to
  -- these windows via winhighlight, so your other diffs keep their look.
  vim.api.nvim_set_hl(0, "Homm2DiffChange", {})
  vim.api.nvim_set_hl(0, "Homm2DiffText", { underline = true })
  local WH = "DiffChange:Homm2DiffChange,DiffText:Homm2DiffText"

  -- render TARGET first so its paired raw indices are known, then the BASE pane
  -- drives the overlay: statement text on base + an empty `;` on target at the
  -- index-paired row, keeping the scrollbound panes aligned.
  local tbuf, traws = pane("target", t)
  local bbuf = pane("base", b, { sibling_target = tbuf, traws = traws })
  vim.cmd(M.config.split)
  vim.api.nvim_win_set_buf(0, tbuf)
  set_winbar(vim.api.nvim_get_current_win(), "TARGET (retail)", sym.name, t.match_percent)
  vim.cmd("diffthis")
  vim.wo[0].foldenable = false   -- show the WHOLE function, not just changed hunks
  vim.wo[0].winhighlight = WH
  vim.cmd("rightbelow vsplit")
  vim.api.nvim_win_set_buf(0, bbuf)
  set_winbar(vim.api.nvim_get_current_win(), "BASE (recompiled)", sym.name, b.match_percent)
  vim.cmd("diffthis")
  vim.wo[0].foldenable = false
  vim.wo[0].winhighlight = WH
  vim.cmd("normal! gg")
end

-- --------------------------------------------------------------- entry pts ---

local function with_root(fn)
  local root = project_root(0)
  if not root then
    return notify("no " .. ROOT_MARKER .. " above this file - not a homm2 checkout",
      vim.log.levels.ERROR)
  end
  return fn(root)
end

-- Close any open asm/diff view windows so a new view REPLACES the previous one
-- (e.g. vd on a new function closes the old vd) instead of stacking splits.
local function close_view_windows()
  for _, w in ipairs(vim.api.nvim_list_wins()) do
    if vim.api.nvim_win_is_valid(w) then
      local n = vim.api.nvim_buf_get_name(vim.api.nvim_win_get_buf(w))
      if n:match("^homm2://target/") or n:match("^homm2://base/")
          or n:match("^homm2://diff%.") then
        pcall(vim.api.nvim_win_close, w, true)
      end
    end
  end
end

--- Render the target/base/diff view for `sym`. `src_win`: the window to return
--- focus to (so vt/vb/vd don't move the cursor out of the source buffer).
local function open_for(root, sym, side, src_win)
  unit_diff(root, sym.unit, sym.name, function(json)
    local t, b = pick(json, "left", sym.name), pick(json, "right", sym.name)
    local one = (side == "target") and t or (side == "base") and b or nil
    if side == "diff" and not (t and b) then
      return notify(sym.name .. " not in the built objs - rebuild?", vim.log.levels.WARN)
    end
    if side ~= "diff" and not one then
      return notify(sym.name .. " has no " .. side .. " obj - rebuild?", vim.log.levels.WARN)
    end
    close_view_windows()   -- one view at a time: replace any previous asm/diff view
    if side == "diff" then
      show_diff(root, sym, t, b)
    else
      local asm, addrs = asm_lines(one)
      local lines = { ("; %s   %s   %s"):format(sym.name, side, pct(one.match_percent)) }
      vim.list_extend(lines, asm)
      -- stash the rendered asm so a later build can diff base prev-build -> now.
      -- line_offset = 1: the header line above shifts every asm row down one.
      show_split(lines, { root = root, tag = side .. "/" .. sym.name, kind = "asm",
                          name = sym.name, unit = sym.unit, side = side, asm = asm,
                          addrs = addrs, line_offset = 1 })
    end
    -- keep the cursor in the source buffer; don't jump into the opened view
    if src_win and vim.api.nvim_win_is_valid(src_win) then
      vim.api.nvim_set_current_win(src_win)
    end
  end)
end

--- :Homm2 {target|base|diff} on the function at the cursor.
function M.view(side)
  with_root(function(root)
    local src_win = vim.api.nvim_get_current_win()
    local sym, err = symbol_at(root, 0, vim.api.nvim_win_get_cursor(0)[1])
    if not sym then return notify(err, vim.log.levels.WARN) end
    open_for(root, sym, side, src_win)
  end)
end

--- V: peek the function-at-cursor's match % + metadata in a float. Instant -
--- pure file reads (report.json + symbol_names.csv), no objdiff subprocess.
--- Inside a plugin view, V/<CR> follow the address/function under the cursor.
function M.peek()
  if vim.b.homm2 then return M.follow() end
  with_root(function(root)
    local sym, err = symbol_at(root, 0, vim.api.nvim_win_get_cursor(0)[1])
    if not sym then return notify(err, vim.log.levels.WARN) end
    local e = (load_symbols(root) or { by_addr = {} }).by_addr[tonumber(sym.addr)] or {}
    local rep = load_report(root)
    local u = rep and rep.units[sym.unit]
    local lp = live_pct[root] and live_pct[root][sym.unit]  -- fresher than report
    local p = (lp and lp[sym.name]) or (u and u.funcs[sym.name])
    local sz = tonumber(e.size)

    local lines = { sym.name, "" }
    lines[#lines + 1] = ("  match   %s"):format(
      p and (pct(p) .. (p >= 100 and "   EXACT" or "")) or "n/a (run :Homm2Build)")
    if sz then lines[#lines + 1] = ("  size    %d B (%s)"):format(sz, e.size) end
    lines[#lines + 1] = ("  rva     %s"):format(sym.addr)
    if e.kind and e.kind ~= "func" then
      lines[#lines + 1] = ("  kind    %s"):format(e.kind)
    end
    lines[#lines + 1] = u
      and ("  unit    %s   %s  (%d/%d fns)"):format(sym.unit, pct(u.pct), u.mf, u.tf)
      or  ("  unit    %s"):format(sym.unit)

    show_float(lines, { root = root, filetype = "", name = sym.name,
      title = p and pct(p) or sym.unit })
  end)
end

--- <CR>/V inside a plugin view: a status row opens that function's diff; an
--- address under the cursor jumps to its function's diff.
function M.follow()
  local ctx = vim.b.homm2
  if not ctx then return end
  if ctx.view == "status" and ctx.func_lines then
    local fl = ctx.func_lines[tostring(vim.fn.line("."))]
    if fl then return open_for(ctx.root, { name = fl.name, unit = fl.unit }, "diff") end
  end
  local word = vim.fn.expand("<cword>")
  local addr = word:match("^0[xX]%x+$")
  if not addr then return notify("no address under cursor", vim.log.levels.INFO) end
  local db = load_symbols(ctx.root)
  local e = db and db.by_addr[tonumber(addr)]
  if not e then return notify(addr .. " is not a known function", vim.log.levels.WARN) end
  open_for(ctx.root, { name = e.name, unit = e.unit }, "diff")
end

--- :Homm2 status - the match-% overview (report.json), the current file's unit
--- expanded to its functions; <CR> on a function opens its diff.
--- Build the status view's lines + the line->function map (for <CR>). Returns
--- nil if there is no report yet. Shared by M.status and the post-build refresh.
local function status_lines(root, cur_unit)
  local rep = load_report(root)
  if not rep then return nil end
  local lines = { ("; homm2 match status - overall %s"):format(pct(rep.overall)), "" }
  local order = vim.deepcopy(rep.order)
  table.sort(order, function(a, b) return rep.units[a].pct < rep.units[b].pct end)
  for _, name in ipairs(order) do
    local u = rep.units[name]
    local mark = (name == cur_unit) and ">" or " "
    lines[#lines + 1] = ("%s %-22s %8s   %d/%d fns"):format(mark, name, pct(u.pct), u.mf, u.tf)
  end
  local func_lines = {}
  if cur_unit and rep.units[cur_unit] then
    lines[#lines + 1] = ""
    lines[#lines + 1] = ("; %s functions (worst first) - <CR> opens diff"):format(cur_unit)
    local fns = {}
    for fn, p in pairs(rep.units[cur_unit].funcs) do fns[#fns + 1] = { fn, p } end
    table.sort(fns, function(a, b) return a[2] < b[2] end)
    for _, fp in ipairs(fns) do
      lines[#lines + 1] = ("    %8s  %s"):format(pct(fp[2]), fp[1])
      func_lines[tostring(#lines)] = { unit = cur_unit, name = fp[1] }
    end
  end
  return lines, func_lines
end

function M.status()
  with_root(function(root)
    local cur_unit = (select(1, symbol_at(root, 0, vim.api.nvim_win_get_cursor(0)[1])) or {}).unit
    local lines, func_lines = status_lines(root, cur_unit)
    if not lines then
      return notify("no " .. REPORT .. " - run :Homm2Build first", vim.log.levels.WARN)
    end
    show_split(lines, { root = root, tag = "status", view = "status", unit = cur_unit,
                        filetype = "asm", func_lines = func_lines })
  end)
end

-- --------------------------------------------------------- inline % hints ---
-- Like coc's inlay hints: each VA(...) line gets its function's match % as
-- end-of-line virtual text, colored by how close it is. Reads the same
-- mtime-cached symbol_names.csv + report.json the views use.
local HINT_NS = vim.api.nvim_create_namespace("homm2_pct")

local function hint_hl(p)
  if not p then return "Comment" end
  if p >= 100 then return "DiagnosticOk" end
  if p >= 50 then return "DiagnosticWarn" end
  return "DiagnosticError"
end

function M.hints(buf)
  buf = buf or vim.api.nvim_get_current_buf()
  if not vim.api.nvim_buf_is_loaded(buf) then return end
  vim.api.nvim_buf_clear_namespace(buf, HINT_NS, 0, -1)
  if not M.config.hints then return end
  local root = project_root(buf)
  if not root then return end
  local db = load_symbols(root)
  if not db then return end
  local rep = load_report(root)
  for i, line in ipairs(vim.api.nvim_buf_get_lines(buf, 0, -1, false)) do
    local a = line:match("VAU?%s*%(%s*(0[xX]%x+)")
    if a then
      local e = db.by_addr[tonumber(a)]
      local lp = e and live_pct[root] and live_pct[root][e.unit]  -- fresher than report
      local p = (lp and lp[e.name])
        or (e and rep and rep.units[e.unit] and rep.units[e.unit].funcs[e.name])
      local txt = (not e) and "?? unknown rva"
        or p == nil and "— n/a"
        or p >= 100 and "✓ 100%"
        or string.format("%.1f%%", p)
      vim.api.nvim_buf_set_extmark(buf, HINT_NS, i - 1, 0, {
        virt_text = { { "  " .. txt, hint_hl(p or nil) } },
        virt_text_pos = "eol", hl_mode = "combine",
      })
    end
  end
end

--- Re-render hints in every loaded buffer (a build changes the %s); non-project
--- buffers early-return inside M.hints.
local function refresh_all_hints()
  for _, b in ipairs(vim.api.nvim_list_bufs()) do M.hints(b) end
end

-- ------------------------------------------------------------------- build ---

--- A compact popup summarising the build: overall + current-unit % deltas and the
--- functions that MOVED (capped). Fades after a few seconds, never steals focus.
--- `vs` has the full per-function table.
local function build_popup(root, before, unit, elapsed)
  local after = read_report(root)
  if not after then return notify("build done (no report.json to compare)") end
  local b = before or { units = {} }
  local lines = { ("overall  %s -> %s"):format(pct(b.overall or 0), pct(after.overall)) }
  local ua, ub = after.units[unit], b.units[unit]
  if unit and ua then
    lines[#lines + 1] = ("%s  %s -> %s  (%d/%d)")
      :format(unit, pct((ub or {}).pct or 0), pct(ua.pct), ua.mf, ua.tf)
    local moved = {}
    for fn, p in pairs(ua.funcs) do
      local was = ub and ub.funcs[fn]
      if was == nil or p ~= was then
        moved[#moved + 1] = { fn = fn, was = was, now = p, d = p - (was or 0) }
      end
    end
    table.sort(moved, function(x, y) return x.d > y.d end)
    if #moved == 0 then
      lines[#lines + 1] = "(no function % changes)"
    else
      for i = 1, math.min(#moved, 6) do
        local r = moved[i]
        local arr = r.was == nil and "+" or r.d > 0 and "^" or r.d < 0 and "v" or " "
        lines[#lines + 1] = ("  %s %s -> %-7s %s"):format(
          arr, r.was and pct(r.was) or "  -  ", pct(r.now),
          (r.fn:gsub("^%?+", ""):gsub("@.*$", "")))
      end
      if #moved > 6 then lines[#lines + 1] = ("  +%d more (vs)"):format(#moved - 6) end
    end
  end
  show_float(lines, { root = root, filetype = "asm", pos = "editor", timeout = 6000,
    title = elapsed and (" build OK  %.1fs "):format(elapsed) or " build OK " })
end

-- Re-render any open asm/diff/status view buffers from fresh data, so a build
-- updates an open `vd`/`vt`/`vb`/`vs` in place (view state is preserved by
-- set_buf_lines). asm/diff buffers are grouped by symbol so objdiff runs once each.
local function refresh_views(root)
  local groups, status_bufs = {}, {}
  for _, buf in ipairs(vim.api.nvim_list_bufs()) do
    local ctx = vim.api.nvim_buf_is_loaded(buf) and vim.b[buf].homm2 or nil
    if ctx and ctx.root == root then
      if ctx.view == "status" then
        status_bufs[#status_bufs + 1] = buf
      elseif (ctx.kind == "asm" or ctx.kind == "diffpane") and ctx.unit and ctx.name then
        local key = ctx.unit .. "\0" .. ctx.name
        groups[key] = groups[key] or { unit = ctx.unit, name = ctx.name, bufs = {} }
        table.insert(groups[key].bufs, { buf = buf, kind = ctx.kind, side = ctx.side })
      end
    end
  end

  for _, g in pairs(groups) do
    unit_diff(root, g.unit, g.name, function(json)
      for _, b in ipairs(g.bufs) do
        local sym = vim.api.nvim_buf_is_valid(b.buf)
          and pick(json, b.side == "target" and "left" or "right", g.name)
        if sym then
          if b.kind == "asm" then
            local ctx = vim.b[b.buf].homm2
            local new = asm_lines(sym)
            local prev = ctx and ctx.asm
            -- base view + a build that actually changed the base => show what your
            -- edit did to the compiled output, as a previous-build -> now diff.
            if b.side == "base" and prev
               and table.concat(prev, "\n") ~= table.concat(new, "\n") then
              local d = vim.diff(table.concat(prev, "\n") .. "\n",
                                 table.concat(new, "\n") .. "\n", { ctxlen = 2 })
              local lines = { ("; %s   base: previous build -> now   %s")
                :format(g.name, pct(sym.match_percent)) }
              vim.list_extend(lines, vim.split(d or "", "\n", { trimempty = true }))
              set_buf_lines(b.buf, lines)
              vim.bo[b.buf].filetype = "diff"
            else
              local lines = { ("; %s   %s   %s"):format(g.name, b.side, pct(sym.match_percent)) }
              vim.list_extend(lines, new)
              set_buf_lines(b.buf, lines)
              vim.bo[b.buf].filetype = "asm"
            end
            if ctx then ctx.asm = new; vim.b[b.buf].homm2 = ctx end
          else
            set_buf_lines(b.buf, asm_lines(sym))
            for _, w in ipairs(vim.fn.win_findbuf(b.buf)) do
              set_winbar(w, b.side == "target" and "TARGET (retail)" or "BASE (recompiled)",
                g.name, sym.match_percent)
            end
          end
        end
      end
    end)
  end

  for _, buf in ipairs(status_bufs) do
    local ctx = vim.b[buf].homm2
    local lines, func_lines = status_lines(root, ctx.unit)
    if lines then
      set_buf_lines(buf, lines)
      ctx.func_lines = func_lines
      vim.b[buf].homm2 = ctx
    end
  end
end

local function finish_build(root, before, unit, code, elapsed)
  if code ~= 0 then
    return notify("build FAILED (exit " .. code .. ")", vim.log.levels.ERROR)
  end
  live_pct[root] = nil  -- a full build refreshed report.json -> drop the per-unit overlay
  invalidate(root)
  diff_cache = {}
  build_popup(root, before, unit, elapsed)
  refresh_all_hints()  -- the inline %s (and status) now reflect the new build
  refresh_all_source()
  refresh_views(root)  -- and any open asm/diff/status views re-render in place
end

-- One build at a time, latest-wins: a new request kills the in-flight build and
-- starts fresh (rapid saves; also avoids two ninja runs racing on build/).
-- build_gen tags each request so a killed job's on_exit bails out.
local build_job, build_gen, build_note = nil, 0, nil

--- Run `homm2 build` (incremental).
--- quiet=false: a terminal split with the live log (manual :Homm2Build).
--- quiet=true : no log window - just a small "building ..." note that closes
---              into the result popup (build-on-save).
--- The plugin only ever loads from a `nix develop` shell (the shim that injects it
--- is itself set up by the shellHook), so the toolchain is on PATH and we run
--- `homm2 build` DIRECTLY. From the wrong shell (.#default - no wine/MSVC) the build
--- fails loudly, which is the correct signal to re-enter `nix develop .#build`.
local function do_build(root, args, quiet)
  if build_job then pcall(vim.fn.jobstop, build_job); build_job = nil end
  if build_note then pcall(build_note); build_note = nil end
  build_gen = build_gen + 1
  local my_gen = build_gen
  local t0 = uv.hrtime()
  local before = read_report(root)
  local unit = (select(1, symbol_at(root, 0, vim.api.nvim_win_get_cursor(0)[1])) or {}).unit
  local cmd = { "homm2", "build" }
  vim.list_extend(cmd, args or {})
  log("build" .. (quiet and " (on save)" or "")
    .. ": " .. table.concat(cmd, " ") .. "  [" .. root .. "]")
  local function on_exit(_, code)
    vim.schedule(function()
      if my_gen ~= build_gen then return end -- superseded by a newer build
      build_job = nil
      if build_note then pcall(build_note); build_note = nil end
      finish_build(root, before, unit, code, (uv.hrtime() - t0) / 1e9)
    end)
  end
  if quiet then
    build_note = show_note("building " .. (unit or "...") .. " ...")
    build_job = vim.fn.jobstart(cmd, { cwd = root, on_exit = on_exit })
  else
    vim.cmd(M.config.split)
    local buf = vim.api.nvim_create_buf(false, true)
    vim.api.nvim_win_set_buf(0, buf)
    build_job = vim.fn.jobstart(cmd, { term = true, cwd = root, on_exit = on_exit })
  end
end

--- :Homm2Build [ninja args] - manual recompile in a terminal split, then a
--- popup reporting what moved for the current file's unit.
function M.build(args)
  with_root(function(root) do_build(root, args, false) end)
end

-- ---------------------------------------------------- fast build-on-save ---
-- A save recompiles ONLY the edited file's unit (one `cl`, ~0.6s) and re-diffs
-- that unit against the cached target objs - it does NOT regenerate the all-units
-- objdiff report itself (the slow, unit-count-scaling step). The target side is
-- fixed by the retail EXE, so a body edit can't change it; delink/report only
-- matter when you add/move/rename a labeled function, which is what a full
-- `:Homm2Build` (vB) is for. Tradeoff: between full builds the overall % and
-- OTHER units' numbers (status view) stay at the last full build; only the edited
-- unit updates live.

--- The unit a source buffer compiles into - the unit of the first VA(...) in it.
--- nil for a file with no matched functions yet (-> caller falls back to a full build).
local function unit_of_buf(root, buf)
  local db = load_symbols(root)
  if not db then return nil end
  for _, line in ipairs(vim.api.nvim_buf_get_lines(buf, 0, -1, false)) do
    local a = line:match("VAU?%s*%(%s*(0[xX]%x+)")
    if a then
      local e = db.by_addr[tonumber(a)]
      if e then return e.unit end
    end
  end
end

--- The unit's effective %s right now: report.json overlaid by any live overlay.
--- Used as the "before" snapshot so the popup shows what THIS save moved.
local function effective_unit_pcts(root, unit)
  local out = {}
  local rep = load_report(root)
  local u = rep and rep.units[unit]
  if u then for fn, p in pairs(u.funcs) do out[fn] = p end end
  local lp = live_pct[root] and live_pct[root][unit]
  if lp then for fn, p in pairs(lp) do out[fn] = p end end
  return out
end

--- Compact corner popup: the unit + the functions this save moved. Same shape as
--- build_popup, but unit-scoped (no overall %, which a fast save doesn't compute).
local function fast_build_popup(root, unit, before, now, elapsed)
  local moved = {}
  for fn, p in pairs(now) do
    local was = before[fn]
    if was == nil or p ~= was then
      moved[#moved + 1] = { fn = fn, was = was, now = p, d = p - (was or 0) }
    end
  end
  table.sort(moved, function(x, y) return x.d > y.d end)
  local lines = {}
  if #moved == 0 then
    lines[1] = "(no function % changes)"
  else
    for i = 1, math.min(#moved, 6) do
      local r = moved[i]
      local arr = r.was == nil and "+" or r.d > 0 and "^" or r.d < 0 and "v" or " "
      lines[#lines + 1] = ("  %s %s -> %-7s %s"):format(
        arr, r.was and pct(r.was) or "  -  ", pct(r.now),
        (r.fn:gsub("^%?+", ""):gsub("@.*$", "")))
    end
    if #moved > 6 then lines[#lines + 1] = ("  +%d more"):format(#moved - 6) end
  end
  show_float(lines, { root = root, filetype = "asm", pos = "editor", timeout = 5000,
    title = (" %s  %.1fs "):format(unit, elapsed or 0) })
end

--- Per-unit FUZZY match %s as {name -> pct}. `-c function_reloc_diffs=none` is the
--- exact setting `objdiff-cli report generate` uses, so these equal report.json's
--- fuzzy_match_percent - the overlay stays consistent with report-derived hints (no
--- jump on first save). Kept separate from unit_diff, whose STRICT diff the asm
--- views render (so vt/vb/vd still show reloc differences). cb({}) on any failure.
local function unit_fuzzy_pcts(root, unit, cb)
  if vim.fn.executable("objdiff-cli") == 0 then return cb({}) end
  local cmd = { "objdiff-cli", "diff", "-p", ODIR, "-u", unit, "-o", "-",
                "--format", "json", "-c", "function_reloc_diffs=none" }
  log("objdiff-cli diff -u " .. unit .. " (fuzzy %s)  [" .. root .. "]")
  vim.system(cmd, { cwd = root, text = true }, function(res)
    vim.schedule(function()
      local ok, json = pcall(vim.json.decode, res.code == 0 and res.stdout or "")
      cb(ok and type(json) == "table" and pcts_from_diff(json) or {})
    end)
  end)
end

--- After the scoped compile: fetch the one unit's fuzzy %s, stash them as the live
--- overlay, re-render the edited file's hints + any open views, and pop the summary.
local function finish_fast_build(root, unit, buf, before, elapsed)
  diff_cache = {}  -- base obj changed; drop cached strict diffs (refresh_views re-fetches)
  unit_fuzzy_pcts(root, unit, function(now)
    live_pct[root] = live_pct[root] or {}
    live_pct[root][unit] = now
    if vim.api.nvim_buf_is_valid(buf) then M.hints(buf) end
    refresh_views(root)  -- open vt/vb/vd for this unit re-render against the new base
    refresh_all_source()
    fast_build_popup(root, unit, before, now, elapsed)
  end)
end

--- Quietly compile JUST `unit`'s base obj (a single ninja target), then refresh
--- that unit's %s. Shares the latest-wins job slot with do_build.
local function do_fast_build(root, unit, buf)
  if build_job then pcall(vim.fn.jobstop, build_job); build_job = nil end
  if build_note then pcall(build_note); build_note = nil end
  build_gen = build_gen + 1
  local my_gen = build_gen
  local t0 = uv.hrtime()
  local before = effective_unit_pcts(root, unit)
  local target = "build/objdiff/base/" .. unit .. ".obj"
  -- A specific ninja target -> ninja builds only that obj.
  local cmd = { "homm2", "build", target }
  log("save-build [" .. unit .. "]"
    .. ": " .. table.concat(cmd, " ") .. "  [" .. root .. "]")
  build_note = show_note("building " .. unit .. " ...")
  build_job = vim.fn.jobstart(cmd, { cwd = root, on_exit = function(_, code)
    vim.schedule(function()
      if my_gen ~= build_gen then return end  -- superseded by a newer save/build
      build_job = nil
      if build_note then pcall(build_note); build_note = nil end
      if code ~= 0 then
        return notify("save-build FAILED (exit " .. code .. ")", vim.log.levels.ERROR)
      end
      finish_fast_build(root, unit, buf, before, (uv.hrtime() - t0) / 1e9)
    end)
  end })
end

--- BufWritePost hook: when build-on-save is on, recompile the edited file's unit
--- and update its %s live. A file with no resolvable unit yet (no RVA - a brand-new
--- TU) falls back to a full build, which wires it into the graph (delink + report).
function M.on_save(buf)
  if not M.config.build_on_save then return end
  local root = project_root(buf)
  if not root then return end
  local unit = unit_of_buf(root, buf)
  if unit then do_fast_build(root, unit, buf)
  else do_build(root, {}, true) end
end

-- ------------------------------------------------------------- format on save --
-- homm2 has no tree-wide `format` CLI command; on save we clang-format JUST the
-- file being worked on, with the root .clang-format (the same config the
-- pre-commit hook uses), scoped to src/ + include/ (never _external/), so it
-- stays whitespace-only / matching-neutral.

--- Is `file` under src/ or include/ (not _external/)? Keeps the on-save formatter
--- to the project's own sources.
local function in_format_scope(root, file)
  if not file or file == "" then return false end
  if file:find(root .. "/src/_external/", 1, true) == 1 then return false end
  for _, sub in ipairs({ "/src/", "/include/" }) do
    local pfx = root .. sub
    if file:sub(1, #pfx) == pfx then return true end
  end
  return false
end

--- BufWritePre hook: when format-on-save is on, clang-format the buffer in place
--- (synchronously, before the write hits disk) so the saved file is formatted in
--- one save. The buffer is only rewritten when formatting actually changed
--- something, and the cursor/scroll position is preserved.
function M.format_on_save(buf)
  if not M.config.format_on_save then return end
  buf = buf or vim.api.nvim_get_current_buf()
  local root = project_root(buf)
  if not root then return end
  local file = vim.api.nvim_buf_get_name(buf)
  if not in_format_scope(root, file) then return end
  if vim.fn.executable("clang-format") == 0 then
    return notify("clang-format not on PATH - launch nvim from `nix develop`",
      vim.log.levels.WARN)
  end

  local cur = vim.api.nvim_buf_get_lines(buf, 0, -1, false)
  -- --assume-filename so --style=file resolves the root .clang-format AND clang
  -- picks the C/C++ language from the extension; piping the buffer (not the
  -- on-disk file) formats unsaved edits too.
  local out = vim.fn.systemlist(
    { "clang-format", "--style=file", "--assume-filename=" .. file },
    table.concat(cur, "\n"))
  if vim.v.shell_error ~= 0 then
    return notify("clang-format failed: " .. table.concat(out, " "),
      vim.log.levels.ERROR)
  end

  if #out == #cur then            -- no-op on already-formatted files: leave the
    local same = true             -- buffer (and its undo history) untouched
    for i = 1, #cur do if out[i] ~= cur[i] then same = false break end end
    if same then return end
  end
  local view = vim.fn.winsaveview()
  vim.api.nvim_buf_set_lines(buf, 0, -1, false, out)
  vim.fn.winrestview(view)
end

-- --------------------------------------------------------------- :Homm2Log --

function M.show_log()
  local lines = #M._log > 0 and vim.deepcopy(M._log) or { "(no queries yet)" }
  table.insert(lines, 1, "objdiff-cli: " ..
    (vim.fn.exepath("objdiff-cli") ~= "" and vim.fn.exepath("objdiff-cli")
     or "NOT ON PATH"))
  show_split(lines, { root = project_root(0), tag = "log", filetype = "" })
end

-- ------------------------------------------------------------ setup / maps ---

local warned
--- Warn once, on a c/cpp buffer in a homm2 checkout, if objdiff-cli is missing
--- (usual cause: nvim launched outside `nix develop`).
function M.check(buf)
  if warned or not project_root(buf) then return end
  if vim.fn.executable("objdiff-cli") == 0 then
    warned = true
    notify("`objdiff-cli` not on PATH - launch nvim from `nix develop` for "
      .. ":Homm2 views to work", vim.log.levels.WARN)
  end
end

-- Persist the runtime toggles (hints / build-on-save) so they survive a restart.
-- Per-project, under build/ (already gitignored) - each checkout/worktree
-- remembers its own toggles. Loaded once per root.
local loaded_roots = {}
local function state_path(root) return root .. "/build/homm2-nvim.json" end

local function save_state(root)
  if not root then return end
  vim.fn.mkdir(root .. "/build", "p")
  local fd = io.open(state_path(root), "w")
  if not fd then return end
  fd:write(vim.json.encode({ hints = M.config.hints,
                             build_on_save = M.config.build_on_save,
                             format_on_save = M.config.format_on_save,
                             source_lines = M.config.source_lines }))
  fd:close()
end

--- Apply a checkout's persisted toggle state over the current config (a
--- remembered toggle wins over the setup default). Loads once per root; call
--- with a buffer that lives in that checkout.
function M.load_state(buf)
  local root = project_root(buf or 0)
  if not root or loaded_roots[root] then return end
  loaded_roots[root] = true
  local fd = io.open(state_path(root), "r")
  if not fd then return end
  local ok, s = pcall(vim.json.decode, fd:read("*a"))
  fd:close()
  if ok and type(s) == "table" then
    if type(s.hints) == "boolean" then M.config.hints = s.hints end
    if type(s.build_on_save) == "boolean" then M.config.build_on_save = s.build_on_save end
    if type(s.format_on_save) == "boolean" then M.config.format_on_save = s.format_on_save end
    if type(s.source_lines) == "boolean" then M.config.source_lines = s.source_lines end
  end
end

--- Close every open homm2 view window (vt/vb/vd/vs/build/log).
function M.close()
  for _, w in ipairs(vim.api.nvim_list_wins()) do
    if vim.api.nvim_win_is_valid(w)
        and vim.api.nvim_buf_get_name(vim.api.nvim_win_get_buf(w)):match("^homm2://") then
      pcall(vim.api.nvim_win_close, w, true)
    end
  end
end

function M.complete()
  return { "target", "base", "diff", "status", "hints", "autobuild", "autoformat", "close" }
end

function M.dispatch(arg)
  if arg == "status" then return M.status() end
  if arg == "close" then return M.close() end
  if arg == "hints" then
    M.config.hints = not M.config.hints
    save_state(project_root(0)); refresh_all_hints()
    return notify("inline % hints " .. (M.config.hints and "on" or "off"))
  end
  if arg == "autobuild" then
    M.config.build_on_save = not M.config.build_on_save
    save_state(project_root(0))
    return notify("build on save " .. (M.config.build_on_save and "ON" or "off"))
  end
  if arg == "autoformat" then
    M.config.format_on_save = not M.config.format_on_save
    save_state(project_root(0))
    return notify("format on save " .. (M.config.format_on_save and "ON" or "off"))
  end
  if arg == "source" then
    M.config.source_lines = not M.config.source_lines
    save_state(project_root(0))
    refresh_all_source()   -- re-apply/clear on every open view (both diff panes)
    return notify("source overlay " .. (M.config.source_lines and "ON (base views)" or "off"))
  end
  if arg == "target" or arg == "base" or arg == "diff" then return M.view(arg) end
  return notify("usage: :Homm2 {target|base|diff|status|hints|source|autobuild|autoformat|close}",
    vim.log.levels.WARN)
end

function M.attach_keymaps(buf)
  local maps = {
    vt = function() M.view("target") end,  -- view target asm
    vb = function() M.view("base") end,    -- view base asm
    vd = function() M.view("diff") end,    -- view diff (the objdiff look)
    vs = function() M.status() end,        -- status overview
    vL = function() M.dispatch("source") end, -- toggle source-statement overlay (base views)
    vB = function() M.build({}) end,       -- build
    vq = function() M.close() end,         -- close all homm2 views
  }
  for lhs, fn in pairs(maps) do
    vim.keymap.set("n", lhs, fn, { buffer = buf, silent = true, desc = "homm2 " .. lhs })
  end
  vim.keymap.set("n", "V", function() M.peek() end,
    { buffer = buf, silent = true, desc = "homm2: peek % + metadata" })
end

function M.setup(opts)
  M.config = vim.tbl_deep_extend("force", M.config, opts or {})
  -- per-project state is loaded on the first source buffer (it needs a root)
end

return M
