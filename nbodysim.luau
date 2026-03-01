-- NOTE: Naive implementation
-- because im honestly kinda lazy
-- so uh dont spawn too many bodies

local RunService = game:GetService("RunService")
local DBG = game:GetService("DebugVisualService")

local random = Random.new()

local container = Instance.new("Folder")
container.Parent = workspace

-- whether or not to show debug stuff
-- DEBUG_SHOW_VELOCITY_ARROWS whether or not to show velocity arrows for each body that can move
local DEBUG_ENABLED = true
local DEBUG_SHOW_VELOCITY_ARROWS = false

-- maximum deltaTime for StepPhysics()
-- set to -1 for no max step time
local MAX_STEP_TIME = 0.025

-- gravity constant
-- in short: multiplies the acceleration of bodies
local G = 1

-- softening for gravitational forces
-- if SOFTENING > 0 then division by 0 will never occur
local SOFTENING = 0.1

local RunSimulation = false

local function GetMass(obj)
	return obj.Size.Magnitude ^ 3
end

local function GetSize(mass)
	return Vector3.one * (mass ^ (1 / 3))
end

local function CreateBody(mass, position)
	local body = Instance.new("Part")
	body.Size = GetSize(mass)
	body.Position = position
	body.Velocity = Vector3.zero
	body.Shape = "Ball"

	body.Anchored = false
	body.Parent = container

	return body
end

local function OrbitalVelocityEccentric(center, satellite, eccentricity, theta, normal)
	local direction = satellite.Position - center.Position
	local distance = direction.Magnitude

	local a = distance / (1 - eccentricity * math.cos(theta))
	local mu = G * (GetMass(center) + GetMass(satellite))
	local velocity = math.sqrt(mu * ((2/distance )- (1/a)))

	return normal:Cross(direction.Unit).Unit * velocity
end

local function StepPhysics(deltaTime)
	local bodies = container:GetChildren()
	for i = 1, #bodies do
		local body = bodies[i]
		if body.Anchored then continue end

		local force = Vector3.zero
		for j = i + 1, #bodies do
			local otherBody = bodies[j]
			local direction = otherBody.Position - body.Position
			local distance = direction.Magnitude
			local distanceSquare = (distance ^ 2) + SOFTENING

			local gravityForce = G * ((GetMass(body) * GetMass(otherBody)) / distanceSquare)

			force = force + (direction * gravityForce)
		end

		local acceleration = force * (1 / GetMass(body))
		body.Velocity = body.Velocity + (acceleration * deltaTime)
		body.Position = body.Position + (body.Velocity * deltaTime)
	end
end

-- central body
local centerBody = CreateBody(1000, Vector3.zero)
centerBody.Anchored = true

-- spawn a bunch of bodies
for i = 1, 100 do
	local mass = math.sqrt(math.random(1, 100))

	local position = Vector3.new(
		math.random(-200, 200),
		math.random(-200, 200),
		math.random(-200, 200)
	)

	local body = CreateBody(mass, position)
	body.Velocity = random:NextUnitVector() * 50
end

RunService.PreRender:Connect(function(dt)
	local stepTime = dt

	if MAX_STEP_TIME ~= -1 then
		stepTime = math.min(stepTime, MAX_STEP_TIME)
	end

	if RunSimulation then
		StepPhysics(stepTime)
	end

	if DEBUG_ENABLED then
		DBG:DrawText(`GRAVITY CONST: {G}`, UDim2.fromOffset(40, 40), Color3.new(0, 0.1, 0.2))
		DBG:DrawText(`SOFTENING: {SOFTENING}`, UDim2.fromOffset(40, 60), Color3.new(0, 0.1, 0.2))
		DBG:DrawText(`N-BODY COUNT: {#container:GetChildren()}`, UDim2.fromOffset(40, 80), Color3.new(0, 0.1, 0.2))
		DBG:DrawText(`STEP TIME: {string.format("%.5f", stepTime)}`, UDim2.fromOffset(40, 100), Color3.new(0, 0.1, 0.2))
		DBG:DrawText(`VEL ARROWS SHOWN: {DEBUG_SHOW_VELOCITY_ARROWS and "Y" or "N"}`, UDim2.fromOffset(40, 120), Color3.new(0, 0.1, 0.2))

		if DEBUG_SHOW_VELOCITY_ARROWS then
			for _, body in container:GetChildren() do
				if body.Anchored then continue end
				DBG:DrawArrow3D(body.Position, body.Position + (body.Velocity * 0.25), 1, 4, Color3.new(0, 1, 0))
			end
		end
	end
end)

task.wait(2)
RunSimulation = true