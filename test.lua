require "pilot"

if not pilot.worldsize then
	pilot.configure()
end

function hello(self, n)
	print(pilot.getName(), n)

        local msg = pilot.read(self.chan)
        while msg ~= nil do
              print(pilot.getName(), msg)
              if type(msg) == "function" then
                 print("", msg())
              elseif type(msg) == "table" then
                 for k,v in pairs(msg) do
                     print("", k, v)
                 end
              end
              msg = pilot.read(self.chan)
        end

        return 0
end

local procs = {}
for i=2,pilot.worldsize do
	procs[i] = pilot.process(hello, i, "hello")
	pilot.setName(procs[i], "P-Hello-"..i)
	procs[i].chan = pilot.channel(pilot.main, procs[i])
end

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

for _,message in ipairs(messages) do
    for k,v in pairs(procs) do
        pilot.write(v.chan, message)
    end
end

for _,proc in pairs(procs) do
    pilot.write(proc.chan, nil)
end

print(pilot.endTime())
pilot.stopMain(0)
