local DebugVisualService = game:GetService("DebugVisualService") -- custom service
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

-- you can create signals and use them
local s = Signal.new()

s:Connect(function(...)
	print(...)
end)

s:Fire(0.5, "hello", 12345)

-- blocks in a uniform sphere
-- demonstrates the Random library's NextUnitVector
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

-- gui
local screenGui = Instance.new("ScreenGui")
screenGui.Parent = game.StarterGui

local frame = Instance.new("Frame")
frame.Position = UDim2.new(0.5, 0, 0.5, 0)
frame.Size = UDim2.new(0.2, 0, 0, 2)
frame.AnchorPoint = Vector2.new(0.5, 0.5)
frame.Parent = screenGui

-- text label
local txtLabel = Instance.new("TextLabel")
txtLabel.Position = UDim2.fromScale(1, 1)
txtLabel.Size = UDim2.fromOffset(250, 50)
txtLabel.Text = "Hello I am text label"
txtLabel.AnchorPoint = Vector2.new(1, 1)
txtLabel.Parent = screenGui

-- rotating block
local origin = Vector3.new(0, 10, 0)
local part = Instance.new("Part")
part.Size = Vector3.new(1, 1, 1)
part.Parent = workspace

local t = 0
RunService.PreRender:Connect(function(deltaTime)
	t += deltaTime

	frame.Rotation = math.deg(t)

	part.Position = origin + Vector3.new(math.cos(t), 0, math.sin(t)) * 10
	part.Rotation = Vector3.one * math.deg(t)

	-- temp variable pointing to service
	-- means less typing
	local DBG = DebugVisualService

	DBG:DrawText("FPS: "..math.round(1/deltaTime), UDim2.fromOffset(50, 50))
	DBG:DrawText("There's already an FPS counter", UDim2.fromOffset(50, 70))
	DBG:DrawText("Example text", UDim2.fromOffset(50, 90))

	DBG:DrawLine2D(
		UDim2.fromOffset(0, 0),
		UDim2.fromOffset(20, 200),
		Color3.new(1, 0, 0)
	)

    DBG:DrawLine3D(
        Vector3.zero,
        part.Position,
        Color3.new(0, 1, 0)
    )

	DBG:DrawRectangle(
		UDim2.fromOffset(50, 130),
		UDim2.fromOffset(400, 200),
		Color3.new(0, 0.5, 1),
		0.6
	)

	DBG:DrawRectangleOutline(
		UDim2.fromOffset(50, 130),
		UDim2.fromOffset(400, 200),
		Color3.new(0, 0.5, 1)
	)

	DBG:DrawText("Yoo look at this", UDim2.fromOffset(60, 140))
	DBG:DrawText("I should make later draw calls", UDim2.fromOffset(60, 160))
	DBG:DrawText("draw over earlier draw calls", UDim2.fromOffset(60, 180))
	DBG:DrawText("Right now order is based on", UDim2.fromOffset(60, 200))
	DBG:DrawText("what kind of function is called", UDim2.fromOffset(60, 220))
end)