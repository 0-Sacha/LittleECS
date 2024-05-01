#include "LittleECS/LittleECS.h"

#include "ProjectCore/FLog.h"

ProjectCore::FLog::BasicLogger Logger("Workflow");

/****** Components ******/
struct ASmallComponent
{
    std::array<int, 1> Buffer;
};

struct ABigComponent
{
    std::array<int, 4'096> Buffer;
};

struct Name
{
    std::string Name;
};
// To be able to Format the struct Name
PROJECTCORE_AUTO_FORMATTER(Name, "{}", value.Name);

int main()
{
    /****** The Registry to use ******/
    LECS::Registry registry;


    /****** Entity Ids (Trivially copiable) ******/
    LECS::EntityId alice = registry.CreateEntityId();
    LECS::EntityId bob = registry.CreateEntityId();


    /****** Add Components ******/
    registry.Add<int>(alice, 42);
    registry.Add<int>(bob, 7);

    /**
     * Constructors parameters are Forwarded
    */
    registry.Add<Name>(alice, "Alice");
    registry.Add<Name>(bob, "Bob");

    /****** Get Components ******/
    std::cout << std::endl;
    Logger.Info("Alice's int: {}", registry.Get<int>(alice));
    Logger.Info("Bob's int: {}", registry.Get<int>(bob));


    /****** ForEachUniqueComponent ******/
    std::cout << std::endl;
    /**
     * To ForEach on a specific Component, use ForEachUniqueComponent.
     * It takes an lambda which can contains optionally an `LECS::EntityId` as first parameter, in that case the entityId will be send to the lambda
     * The component you want to loop over is take as an template argument, and the lambda can take the component as:
     *  - value <int>
     *  - const value <const int>
     *  - const reference <const int&>
     *  - reference <int&> **Only if the registry is non-const**
     * 
     * It will loop on all entities that have the specified component.
    */
    Logger.Info("ForEachUniqueComponent (With Entity Id):");
    registry.ForEachUniqueComponent<Name>(
        [](LECS::EntityId entityId, const Name& name)
        {
            Logger.Info("    {} -> {}", entityId, name);
        }  
    );

    std::cout << std::endl;
    Logger.Info("ForEachUniqueComponent (Without Entity Id):");
    registry.ForEachUniqueComponent<Name>(
        [](const Name& name)
        {
            Logger.Info("    {}", name);
        }  
    );


    /****** Add Custom Components ******/
    std::cout << std::endl;
    /**
     * You can add any Custom Component as any others types. No need for these components to herit from anythings
    */
    registry.Add<ABigComponent>(alice);
    registry.Add<ASmallComponent>(alice);
    registry.Add<ABigComponent>(bob);

    /****** ForEachComponents ******/
    std::cout << std::endl;
    /**
     * To ForEach on a multiple components (only one included), use ForEachComponents.
     * Like `ForEachUniqueComponent`:
     * It takes an lambda which can contains optionally an `LECS::EntityId` as first parameter, in that case the entityId will be send to the lambda
     * The component you want to loop over is take as an template argument, and the lambda can take the component as:
     *  - value <int>
     *  - const value <const int>
     *  - const reference <const int&>
     *  - reference <int&> **Only if the registry is non-const**
     * 
     * It will loop on all entities that have **ALL** the specified components.
    */
    Logger.Info("ForEachComponents (Without Entity Id) on <Name, int>:");
    registry.ForEachComponents<Name, int>(
        [](const Name& name, const int k)
        {
            Logger.Info("    {} -> {}", name, k);
        }  
    );

    std::cout << std::endl;
    /**
     * Here both Alice and Bob have <int> and <ABigComponent>, so they will be both printed
    */
    Logger.Info("ForEachComponents (With Entity Id) on <Name, int, ABigComponent>:");
    Logger.Debug("Both Alice and Bob have <Name> <int> and <ABigComponent>");
    registry.ForEachComponents<Name, int, ABigComponent>(
        [](LECS::EntityId entityId, const Name& name, const int k, const ABigComponent&)
        {
            Logger.Info("    {} ({}) -> {}", name, entityId, k);
        }  
    );

    std::cout << std::endl;
    /**
     * Here only Alice all 3 components required: <ASmallComponent> <int> <ABigComponent> so she will be printed.
     * Bob doesn't have a <ASmallComponent> so he will not printed
    */
    Logger.Info("ForEachComponents (With Entity Id) on <Name, ASmallComponent, int, ABigComponent> :");
    Logger.Debug("Only Alice has <ASmallComponent> so Bob is not printed. Alice does appear since she has all required components");
    registry.ForEachComponents<Name, ASmallComponent, int, ABigComponent>(
        [](LECS::EntityId entityId, const Name& name, const ASmallComponent&, const int k, const ABigComponent&)
        {
            Logger.Info("    {} ({}) -> {}", name, entityId, k);
        }  
    );

    /****** EachEntitiesWith ******/
    std::cout << std::endl;
    /**
     * EachEntitiesWith will loop over ever entities that have all components required (here only <Name>)
    */
    Logger.Info("EachEntitiesWith <Name>:");
    for (LECS::EntityId entityId : registry.EachEntitiesWith<Name>())
    {
        Logger.Info("    {} -> {}", registry.Get<Name>(entityId),
            registry.Has<ASmallComponent>(entityId) ? "Has <ASmallComponent>" : "Does NOT has <ASmallComponent>"
        );
    }


    /****** Views ******/
    std::cout << std::endl;
    Logger.Info("Views");
    
    /**
     * You can create views on components:
     * Here a view on <int> and <ABigComponent>
     * It will only be able to view thoses components, any references to another component will end up in 'constraint was not satisfied' error
    */
    auto view = registry.View<Name, int>();

    /**
     * With view, you c=have access to all previous defined For and Each functions
     * Except this time you cannot reference components that where not present in the constructor 
    */
    Logger.Info("View: Entity with the component <Name>");
    for (LECS::EntityId entityId : view.EachEntitiesWith<Name>())
    {
        Logger.Info("    {} ({})", registry.Get<Name>(entityId), entityId);
    }

    LECS::EntityId eve = registry.CreateEntityId();
    registry.Add<Name>(eve, "Eve");

    std::cout << std::endl;
    Logger.Info("View doesn't need to be recreated, they are always up to date (After adding Eve):");
    for (LECS::EntityId entityId : view.EachEntitiesWith<Name>())
    {
        Logger.Info("    {} ({})", registry.Get<Name>(entityId), entityId);
    }

    std::cout << std::endl;
    /**
     * With view, you can use the regular for each loop on components.
     * And use C++17 structural bindings.
     * Here Eve will not appear since she doesn't have an <int> component
    */
    Logger.Info("Views: EachComponents");
    for (const auto& [name, intComponent] : view.EachComponents<Name, int>())
    {
        Logger.Info("    {} -> {}", name, intComponent);
    }

    registry.Add<int>(eve, 123);

    /**
     * Again, no need to recreate the view, a view is always up to date
    */
    std::cout << std::endl;
    Logger.Info("View after adding an <int> component to Eve");
    for (const auto& [name, intComponent] : view.EachComponents<Name, int>())
    {
        Logger.Info("    {} -> {}", name, intComponent);
    }

    /**
     * To destroy an entity, just use DestroyEntityId
    */
    std::cout << std::endl;
    registry.DestroyEntityId(bob);

    Logger.Info("View after destroying Bob");
    for (const auto& [name, intComponent] : view.EachComponents<Name, int>())
    {
        Logger.Info("    {} -> {}", name, intComponent);
    }
}
