-- Scene

local function randFloat(min,max)
	return math.min(min,max) + math.random() * math.abs(max-min)
end

for i = 0, 20 do
	glimmer.scene.addSphere(math.random(-100,100), math.random(-32,64), math.random(0,50), math.random(4,12), false)
end
glimmer.scene.addSphere(-35, 82, 130, 64, false)

glimmer.scene.addPlane(0,1,0,0,-100,0,false)

-- Lights
for i = 0, 6 do
	glimmer.scene.addLight(math.random(-250,250),math.random(200,500),math.random(-250,250),
	randFloat(0.0,0.25),randFloat(0.0,0.25),randFloat(0.0,0.25))
end

-- Sky
glimmer.scene.setSkyColour(0.8, 0.8, 0.8)
