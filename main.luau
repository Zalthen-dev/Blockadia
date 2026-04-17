local DebugVisualService = game:GetService("DebugVisualService") -- custom service
local RunService = game:GetService("RunService")

print("Hello World!")

local random = Random.new()

-- baseplate
local baseplate = Instance.new("Part")
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(2048, 16, 2048)
baseplate.Color = Color3.fromRGB(99, 95, 98)
baseplate.Parent = workspace

-- bunch of blocks in a line
for i = 1, 100 do
	local part = Instance.new("Part")
	
	part.Position = Vector3.new(i * 10, 10, 20)

	part.Size = Vector3.new(
		random:NextNumber(1, 10), 
		random:NextNumber(1, 10),
		random:NextNumber(1, 10)
	)

	part.Rotation = Vector3.new(
		random:NextNumber(-360, 360), 
		random:NextNumber(-360, 360),
		random:NextNumber(-360, 360)
	)

	part.Parent = workspace
end

-- you can create signals and use them
local s = Signal.new()

s:Connect(function(...)
	print(...)
end)

s:Fire(0.5, "hello", 12345, baseplate)

-- table sorting with the Fisher-Yates algorithm
-- demonstrates Random:Shuffle

local exampleTable = {"apple", "banana", "cherry", "orange", "grapefruit", "watermelon", "mango"}

-- helper function
local function printTable(t)
	local s = "{ "

	for i = 1, #t do
		s ..= t[i]
		if i < #t then
			s ..= ", "
		end
	end

	s ..= " }"
	print(s)
end

printTable(exampleTable)
random:Shuffle(exampleTable)
printTable(exampleTable)

-- blocks in a uniform sphere
-- demonstrates the Random library's NextUnitVector
local sphereOrigin = Vector3.new(0, 100, 0)
local sphereRadius = 20
for i = 1, 1000 do
	local part = Instance.new("Part")
	part.Size = Vector3.new(1, 1, 1)
	part.Position = sphereOrigin + random:NextUnitVector() * sphereRadius;
	part.Rotation = Vector3.new(
		random:NextNumber(-360, 360),
		random:NextNumber(-360, 360),
		random:NextNumber(-360, 360)
	)
	part.Parent = workspace
end

-- gui
local screenGui = Instance.new("ScreenGui")
screenGui.Parent = game.StarterGui

local frame = Instance.new("Frame")
frame.Position = UDim2.new(0.5, 0, 0.5, 0)
frame.Size = UDim2.new(0, 200, 0, 20)
frame.AnchorPoint = Vector2.new(0.5, 0.5)
frame.Parent = screenGui

local corner = Instance.new("UICorner")
corner.CornerRadius = UDim.new(0, 16)
corner.Parent = frame

local frame2 = frame:Clone()
frame2.Size = UDim2.new(0, 100, 0, 100)
frame2.Position = UDim2.new(0.5, 16, 0.5, 16)
frame2.BackgroundColor3 = Color3.new(1, 0, 0)
frame2.Parent = screenGui

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

-- sound
local sound = Instance.new("Sound")
sound.SoundId = "laser gun sound.ogg"
sound.Volume = 1
sound.PlaybackSpeed = 1
sound.Parent = workspace
--sound:Play()

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
	DBG:DrawText("Workspace objects below", UDim2.fromOffset(60, 160))

	for i, obj in workspace:GetChildren() do
		DBG:DrawText(obj.Name, UDim2.fromOffset(60, 160 + (i*20)))
	end
end)