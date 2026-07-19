local root = assert(vim.env.HOMM2_TEST_ROOT, "HOMM2_TEST_ROOT is required")
root = vim.uv.fs_realpath(root) or vim.fs.normalize(root)

vim.opt.runtimepath:prepend(root .. "/editor/nvim")
vim.cmd.edit(vim.fn.fnameescape(root .. "/flake.nix"))

local actual = require("homm2")._project_root(0)
assert(actual == root, ("expected %s, got %s"):format(root, tostring(actual)))
