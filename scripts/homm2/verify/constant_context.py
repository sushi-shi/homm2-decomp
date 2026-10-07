"""homm2.verify.constant_context - AST destination identities of the values
`homm2 verify enum-reuse` follows (ported from HoMM1's module of that name).

Keys name declarations, not their display names or numeric values.  Operations
between a value and its destination remain in the key: a bit mask used to build
an argument is not evidence that it names the entire argument domain.
"""

from __future__ import annotations


def _contains(outer, inner, ancestors):
    # Macro-expanded siblings can have identical source extents. Only the
    # cursor's actual ancestor path establishes which argument/operand owns it.
    return outer == inner or any(outer == parent for parent in ancestors)


def _identity(node):
    ref = node.referenced
    if ref is None:
        ref = node
    usr = ref.get_usr()
    if not usr:
        return None
    owner = ref.semantic_parent
    label = ref.displayname or ref.spelling
    if owner and owner.spelling and owner.kind.name != "TRANSLATION_UNIT":
        label = owner.spelling + "::" + label
    return usr, label


def _operand(cidx, node):
    """Only transparent wrappers preserve the identity of an operand."""
    while node.kind in (cidx.CursorKind.UNEXPOSED_EXPR,
                        cidx.CursorKind.PAREN_EXPR):
        children = list(node.get_children())
        if len(children) != 1:
            return None
        node = children[0]
    if node.kind in (cidx.CursorKind.MEMBER_REF_EXPR,
                     cidx.CursorKind.DECL_REF_EXPR,
                     cidx.CursorKind.CALL_EXPR):
        return _identity(node)
    return None


def semantic_context(cidx, node, stack):
    """Return (stable key, readable destination) for a source value use.

    ``stack`` contains ancestors, outermost first, excluding ``node``.  The
    nearest meaningful destination wins.  Unknown expressions stay distinct
    by source location instead of creating a spurious shared domain.
    """
    operations = []

    def result(kind, identity, suffix=""):
        key, label = identity
        path = "/".join(reversed(operations))
        return (f"{kind}:{key}{suffix}" + (f"/via:{path}" if path else ""),
                f"{kind} {label}{suffix}" + (f" via {path}" if path else ""))

    for parent in reversed(stack):
        kind = parent.kind
        if kind == cidx.CursorKind.CALL_EXPR:
            for index, arg in enumerate(parent.get_arguments()):
                if _contains(arg, node, stack):
                    identity = _identity(parent)
                    if identity:
                        return result("call", identity, f":argument:{index + 1}")
        elif kind in (cidx.CursorKind.BINARY_OPERATOR,
                       cidx.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR):
            children = list(parent.get_children())
            op = parent.spelling
            if len(children) == 2:
                side = 0 if _contains(children[0], node, stack) else 1
                if op in ("=", "+=", "-=", "&=", "|=", "^=") and side == 1:
                    identity = _operand(cidx, children[0])
                    if identity:
                        if op != "=":
                            operations.append(op)
                        return result("value", identity)
                if op in ("==", "!=", "<", ">", "<=", ">=", "&", "|"):
                    identity = _operand(cidx, children[1 - side])
                    if identity:
                        # Equality, assignments and switch labels share the
                        # value domain; bounds and masks have distinct roles.
                        role = "value" if op in ("==", "!=") else f"operand({op})"
                        return result(role, identity)
                operations.append(f"{op or 'binary'}[{side}]")
        elif kind == cidx.CursorKind.CASE_STMT:
            children = list(parent.get_children())
            # Case bodies are not labels; they retain their own destinations.
            if children and _contains(children[0], node, stack):
                switch = next((p for p in reversed(stack)
                               if p.kind == cidx.CursorKind.SWITCH_STMT), None)
                if switch:
                    exprs = list(switch.get_children())
                    identity = _operand(cidx, exprs[0]) if exprs else None
                    if identity:
                        return result("value", identity)
        elif kind in (cidx.CursorKind.VAR_DECL, cidx.CursorKind.FIELD_DECL,
                       cidx.CursorKind.PARM_DECL, cidx.CursorKind.ENUM_CONSTANT_DECL):
            identity = _identity(parent)
            if identity:
                role = "extent" if parent.type.kind == cidx.TypeKind.CONSTANTARRAY else "value"
                if any(p.kind == cidx.CursorKind.INIT_LIST_EXPR for p in stack):
                    role = "initializer"
                return result(role, identity)
        elif kind == cidx.CursorKind.ARRAY_SUBSCRIPT_EXPR:
            children = list(parent.get_children())
            if len(children) == 2 and _contains(children[1], node, stack):
                identity = _operand(cidx, children[0])
                if identity:
                    return result("index", identity)
        elif kind == cidx.CursorKind.RETURN_STMT:
            for owner in reversed(stack):
                if owner.kind in (cidx.CursorKind.FUNCTION_DECL,
                                   cidx.CursorKind.CXX_METHOD):
                    identity = _identity(owner)
                    if identity:
                        return result("return", identity)
        elif kind in (cidx.CursorKind.UNARY_OPERATOR,
                       cidx.CursorKind.CXX_STATIC_CAST_EXPR,
                       cidx.CursorKind.CSTYLE_CAST_EXPR,
                       cidx.CursorKind.CONDITIONAL_OPERATOR):
            operations.append(kind.name)
    return "", "no identified destination"
