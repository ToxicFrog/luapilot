-- test process creation and management

assert(pilot.worldsize > 1, "Need at least 2 nodes to run test suite")

function child(self)
    while true do
        local f = self.to:read()
        if not f then break end
        f()
    end
        return 0
end

function testprocess(name, proc)
    test(name, type(proc) == "PI_PROCESS")
    test(name..".setName", proc.setName == pilot.process.setName)
    test(name..".getName", proc.getName == pilot.process.getName)

    --proc:setName(name)
    --test(name..":setName/getName", proc:getName() == name)
end


processes = { [0] = pilot.main }

testprocess("main", pilot.main)

for i=1,pilot.worldsize-1 do
    local proc = pilot.process(child)
    testprocess("child-"..i, proc)
end
