pilot.startAll()

for _,proc in pairs(tdata.processes) do
	proc.cin:write(function(self) return true; end)
	test("Send code to "..proc:getName(), proc.cout:read())
end

