print("Killbots Tournament")
dofile("serialise.lua")

setArena("Maze1")

totalRounds=500
bots = getRobots()
totalWins = {}
--bots = {"Botman", "ChickenBot", "George Crushington", "Governator", "Hail", "NippleBadger", "Reaper", "landFish", "Vegeta"}
--bots = {"Reaper", "George Crushington"}

for i,b in ipairs(bots) do
	print("   "..b)
	totalWins[b]={wins=0,timeouts=0,draws=0, fired=0, hit=0}
end

print(#bots.." bots total")

resultTable = table.load("tournament.tab")
resultTable = nil
if resultTable == nil then
	resultTable = {}
	for i=1,#bots do
		t = {}
		for j=i+1,#bots do 
			if bots[i]~=bots[j] then
				t[bots[j]] = {}
				print(bots[i].." vs "..bots[j])
			end
		end
		print("t="..#t)
		if next(t) ~= nil then
			resultTable[bots[i]]=t
		end
	end
	table.save(resultTable, "tournament.tab")
end

function getwins(bot1,bot2)
	if resultTable[bot1]~=nil then
		if resultTable[bot1][bot2]~=nil then
			if next(resultTable[bot1][bot2])~=nil then
				return resultTable[bot1][bot2].wins1, resultTable[bot1][bot2].wins2
			end
			return nil
		end
	end
	if resultTable[bot2]~=nil then
		if resultTable[bot2][bot1]~=nil then
			if next(resultTable[bot2][bot1])~=nil then
				return resultTable[bot2][bot1].wins2, resultTable[bot2][bot1].wins1
			end
			return nil
		end
	end
	return nil
end

for b1,vs in pairs(resultTable) do
	for b2,result in pairs(vs) do
		if next(result)==nil then
			wins1 = 0
			wins2 = 0
			activeBots = {b1,b2}
			addRobot(b1)
			addRobot(b2)
			
			for round=0,totalRounds-1 do

				startRound()
				print("Main Loop")
				elapsed = 0
				print("Round "..(round+1).." of "..activeBots[1].." vs "..activeBots[2].." starting")
				while (not isRoundOver()) and (elapsed < 240) do
					update()
					elapsed = elapsed + 1.0/60.0
				end

				a1=isRobotAlive(0)
				a2=isRobotAlive(1)
				
				if a1 and (not a2) then
					wins1 = wins1 + 1
					notifyRobotWon(0,true)
					notifyRobotWon(1,false)
				elseif (not a1) and a2 then
					wins2 = wins2 + 1
					notifyRobotWon(0,false)
					notifyRobotWon(1,true)
				elseif (not a1) and (not a2) then
					wins1 = wins1 + 1
					wins2 = wins2 + 1
					totalWins[b1].draws = totalWins[b1].draws + 1
					totalWins[b2].draws = totalWins[b2].draws + 1
					notifyRobotWon(0,true)
					notifyRobotWon(1,true)
					print("DRAW: "..tostring(a1).." "..tostring(a2).." "..elapsed)
				elseif elapsed >= 240 then
					totalWins[b1].timeouts = totalWins[b1].timeouts + 1
					totalWins[b2].timeouts = totalWins[b2].timeouts + 1
					notifyRobotWon(0,false)
					notifyRobotWon(1,false)
					print("TIMEOUT: "..tostring(a1).." "..tostring(a2).." "..elapsed)
				else
					print("ERROR: Both bots survived without timing out? "..tostring(a1).." "..tostring(a2).." "..elapsed)
				end
				print("Time = "..elapsed)
				ac = getRobotAccuracy(0)
				totalWins[b1].fired = totalWins[b1].fired + ac.fired
				totalWins[b1].hit = totalWins[b1].hit + ac.hit
				ac = getRobotAccuracy(1)
				totalWins[b2].fired = totalWins[b2].fired + ac.fired
				totalWins[b2].hit = totalWins[b2].hit + ac.hit
			end
			resultTable[b1][b2].wins1=wins1
			resultTable[b1][b2].wins2=wins2
			totalWins[b1].wins=totalWins[b1].wins+wins1
			totalWins[b2].wins=totalWins[b2].wins+wins2
			print("RESULT: "..activeBots[1].." vs "..activeBots[2].." is "..wins1..", "..wins2)
			print("TOTAL: "..totalWins[b1].wins..", "..totalWins[b2].wins)
			endRound()
			print("Finished")
			table.save(resultTable, "tournament.tab")

			csv = io.open("results.csv", "w")
			csv:write(",")
			for j=1,#bots do 
				csv:write(bots[j]..",")
			end
			csv:write("\n")
			for i=1,#bots do
				csv:write(bots[i]..",")
				for j=1,#bots do 
					wins1=0
					wins2=0
					
					print(bots[i].." vs "..bots[j].." ji")
					wins1,wins2 = getwins(bots[i],bots[j])
					if wins1~=nil then
						csv:write(wins1.." vs "..wins2..",")
					else
					csv:write(",");
					end
				end
				csv:write("\n")
			end

			csv:write("\n")
			csv:write("\n")
			csv:write(",Total Wins,Timeouts,Draws,Fired,Hit\n")
			for b,w in pairs(totalWins) do
			csv:write(b..","..w.wins..","..w.timeouts..","..w.draws..","..w.fired..","..w.hit.."\n")
			end

			csv:close()
		end
	end
end
