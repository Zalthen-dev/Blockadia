local DebugVisualService = game:GetService("DebugVisualService")
local RunService = game:GetService("RunService")

print("Hello World!")

local random = Random.new()

-- bunch of blocks in a line
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

local s = Signal.new()
s:Connect(function(...)
	print(...)
end)
s:Fire(0.5, "hello", 12345)

-- blocks in a uniform sphere
local sphereOrigin = Vector3.new(0, 100, 0)
local sphereRadius = 20
for i = 1, 1000 do
	local part = Instance.new("Part")
	part.Size = Vector3.new(1, 1, 1)
	part.Position = sphereOrigin + random:NextUnitVector() * sphereRadius;
	part.Rotation = Vector3.new(
		math.random(-360, 360),
		math.random(-360, 360),
		math.random(-360, 360)
	)
	part.Parent = workspace
end

-- rotating block
local origin = Vector3.new(0, 10, 0)
local part = Instance.new("Part")
part.Size = Vector3.new(1, 1, 1)
part.Parent = workspace

local t = 0
RunService.PreRender:Connect(function(deltaTime)
	t += deltaTime

	part.Position = origin + Vector3.new(math.cos(t), 0, math.sin(t)) * 10
	part.Rotation = Vector3.one * math.deg(t)

	DebugVisualService:DrawText("FPS: "..math.round(1/deltaTime), 50, 50)
	DebugVisualService:DrawText("There's already an FPS counter", 50, 70)
	DebugVisualService:DrawText("Example text", 50, 90)

	DebugVisualService:DrawLine2D(
		Vector2.zero,
		Vector2.new(2000, 2000),
		Color3.new(1, 0, 0)
	)

    DebugVisualService:DrawLine3D(
        Vector3.zero,
        part.Position,
        Color3.new(0, 1, 0)
    )
end)