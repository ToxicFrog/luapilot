for _,proc in pairs(tdata.processes) do
	proc.cin:write(false)
	test("Shutting down "..proc:getName(), true)
end

pilot.stopMain(0)
