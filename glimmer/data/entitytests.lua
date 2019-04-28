local function printEntity(handle)
	if(handle:IsValid() == true) then
		entity = handle:GetEntity()
		print("ID: ", entity:GetID(), ", Name: ", entity:GetName())
		for i = 0, entity:ComponentCount()-1 do
			cmp = entity:GetComponent(i)
			print("\t", cmp:GetTypeString())
		end
	else
		print("Empty")
	end
end

local world = myWorld
local entityHandle = world:CreateEntity("entity1")
printEntity(entityHandle)

local entityHandle2 = world:CreateEntity("entity2")
printEntity(entityHandle2)

local entityHandle3 = entityHandle
printEntity(entityHandle3)

local failHandle = world:CreateEntity("entity1")
printEntity(failHandle)

local tc = entityHandle:GetEntity():Create_TestComponent()
tc:Bark()
printEntity(entityHandle)