-- make sure the library loaded properly
funcs = {
    "configure",
    "startAll",
    "stopMain",
    "log",
    "isLogging",
    "startTime",
    "endTime",
    "setName",
    "getName",
    "abort",
    "copyChannels",
    "channelHasData",
    "read",
    "write",
    "select",
    "trySelect",
    "getBundleSize",
    "getBundleChannel",
    "broadcast",
    "gather",
}

tables = {
    process = { "getName", "setName" };
    channel = { "getName", "setName", "hasData", "read", "write" };
    bundle  = { "getName", "setName", "select", "trySelect", "getSize", "getChannel", "broadcast", "gather" };
}

for _,name in ipairs(funcs) do
    test("pilot."..name, type(pilot[name]) == "function")
end

for tt,methods in pairs(tables) do
    test("pilot."..tt, type(pilot[tt]) == "table")
    for _,method in ipairs(methods) do
        test("pilot."..tt..":"..method, type(pilot[tt][method]) == "function")
    end
end
