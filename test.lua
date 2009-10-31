require "luapilot"

if not pilot.worldsize then
	pilot.configure()
end

function hello(self, n)
	print(pilot.getName(), n)

        for msg in function() return self.chan:read() end do
              print(pilot.getName(), msg)

              if type(msg) == "function" then
                 print("", msg())

              elseif type(msg) == "table" then
                 for k,v in pairs(msg) do
                     print("", k, v)
                 end
              end
        end
        print(pilot.getName(), 'Exiting')
        self.reply:send(n)
        return 0
end

local procs = {}
local chans = {}
local rchans = {}

for i=2,pilot.worldsize do
        local p = pilot.process(hello, i, "hello")
        for k,v in pairs(getmetatable(p)) do
        print(k,v)
        end
        print("new process", p)
        p:setName("P-Hello-"..i)
        p.chan = pilot.channel(pilot.main, p)
        p.reply = pilot.channel(p, pilot.main)

        table.insert(chans, p.chan)
        table.insert(rchans, p.reply)
        procs[i] = p
end

local bundle = pilot.bundle("select", rchans)

local messages = {
    1, 2, 3;
    "foo", "bar", "baz";
    true, false;
    function() return true end;
    {}, { "a", "b", "c" };
    setmetatable({}, { __send = function() return "foo" end, __recv =
        function(s) return s:upper() end });
}


pilot.startAll()
pilot.startTime()

print(pilot.main, "main")

for i=1,pilot.worldsize-1 do
    print(pilot.worldsize, i, bundle[i], #chans, #rchans)
end

for _,message in ipairs(messages) do
    for k,v in pairs(procs) do
        v.chan:write(message)
    end
end

for _,proc in pairs(procs) do
    pilot.write(proc.chan, nil)
end

for i=2,pilot.worldsize do
    local c,n = bundle:select()
    print(n, c, c:read())
end

print(pilot.broadcast)
print(bundle.broadcast)

print(pilot.endTime())
pilot.stopMain(0)
