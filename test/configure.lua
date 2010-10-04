local function testf(self)
	while true do
		message = self.cin:read()
		if not message then break end
		assert(type(message) == "function")
		self.cout:write(message(self))
	end
end

tdata.processes = {}

for i=2,pilot.worldsize do
	local proc = pilot.process(testf)
	test("Create process "..i, proc)

	proc:setName("Test Process "..i)
	test("Set process "..i.." name", proc:getName() == "Test Process "..i)

	proc.cin = pilot.channel(pilot.main, proc)
	test("Create process "..i.." input channel", proc.cin)

	proc.cout = pilot.channel(proc, pilot.main)
	test("Create process "..i.." output channel", proc.cout)

	tdata.processes[i] = proc
end
