print("Hello World!")

for i = 1, 100 do
	local part = Instance.new("Part")
	
	part.Position = Vector3.new(i * 10, 10, 20)

	part.Size = Vector3.new(
		math.random(1, 10), 
		math.random(1, 10),
		math.random(1, 10)
	)

	part.Rotation = Vector3.new(
		math.random(-360, 360), 
		math.random(-360, 360),
		math.random(-360, 360)
	)

	part.Parent = workspace
end

local part = Instance.new("Part")
part.Size = Vector3.new(1, 1, 1)
part.Parent = workspace

local t = 0

while true do
	t += task.wait()

	part.Position = Vector3.new(math.cos(t), 0, math.sin(t)) * 10
	part.Rotation = Vector3.one * math.deg(t)
end