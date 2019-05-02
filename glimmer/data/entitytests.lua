local function printEntity(handle)
	if(handle:IsValid() == true) then
		local entity = handle:GetEntity()
		print("ID: ", entity:GetID(), ", Name: ", entity:GetName())
		for i = 0, entity:ComponentCount()-1 do
			local cmp = entity:GetComponentByIndex(i)
			print("\t", cmp:GetTypeString())
		end
	else
		print("Empty")
	end
end

local function EntityTest()
	local entityHandle = myWorld:CreateEntity("entity1")	
	entityHandle:CreateComponent_TestComponent()
	printEntity(entityHandle)
	myWorld:SpawnEntity(entityHandle)

	for i = 0, 10 do
		myWorld:Tick()
	end
    
	 entityHandle:GetComponent_TestComponent():DoSomething()
	 myWorld:DespawnEntity(entityHandle)

	local entityHandle = myWorld:CreateEntity("entity2")	
	entityHandle:CreateComponent_TestComponent()
	printEntity(entityHandle)
	myWorld:SpawnEntity(entityHandle)
    
	for i = 0, 10 do
		myWorld:Tick()
	end

	local entityHandle = myWorld:CreateEntity("entity3")	
	entityHandle:CreateComponent_TestComponent()
	printEntity(entityHandle)
	myWorld:SpawnEntity(entityHandle)
end

EntityTest()