local rank = require("mpirank")()

function printf(...)
    if rank > 0 then return end
    return io.write(string.format(...))
end

local failed = false
function test(name, condition, indent)
    indent = indent or "    "
    
    printf("%s%s%-60s%s\n"
        , condition and "  " or "!!"
        , indent
        , name
        , condition and "PASS" or "FAIL")
    failed = (not failed) or (not condition)
end

function runsuite(name, longname)
    failed = false
    printf("    %-60s\n", longname)

    dofile("test/"..name..".lua")
    
    test(longname, not failed, "  ")
end

-- cross-suite data can be stored here
tdata = {}

-- test that the library loaded properly
runsuite("load", "Library initialization")

assert(pilot.worldsize > 2, "need at least 3 nodes to run test suite")

runsuite("configure", "Configuration phase")
runsuite("start", "Run phase")
runsuite("readwrite", "Read/Write")
runsuite("stop", "Shutdown")
