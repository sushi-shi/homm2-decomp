-- Object properties are native typed userdata in the portable engine.
-- Getters, setters, and callback arguments share the same lifetime checks.
-- A retained temporary callback value or expired battle stack can be checked
-- before use; game objects never expose a numeric native address.
assert(type(IsObjectValid) == "function", "Ironfist object bindings are unavailable")
return { isValid = IsObjectValid }
