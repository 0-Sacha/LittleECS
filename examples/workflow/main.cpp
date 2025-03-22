#include "lecs/lecs.h"
#include "StreamFormat/FLog.h"

StreamFormat::FLog::BasicLogger Logger("Workflow");

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

// To be able to format the struct Name
STREAMFORMAT_AUTO_FORMATTER(Name, "{}", value.Name);

int main()
{
    /****** The Registry to use ******/
    lecs::Registry registry;

    /****** Entity IDs (trivially copiable) ******/
    lecs::EntityId alice = registry.create_entityid();
    lecs::EntityId bob = registry.create_entityid();

    /****** Add Components ******/
    registry.Add<int>(alice, 42);
    registry.Add<int>(bob, 7);

    /**
     * Constructor parameters are forwarded
    */
    registry.Add<Name>(alice, "Alice");
    registry.Add<Name>(bob, "Bob");

    /****** get Components ******/
    std::cout << std::endl;
    Logger.Info("Alice's int: {}", registry.get<int>(alice));
    Logger.Info("Bob's int: {}", registry.get<int>(bob));

    /****** foreach_unique_component ******/
    std::cout << std::endl;
    /**
     * To ForEach on a specific component, use foreach_unique_component.
     * It takes a lambda which can optionally contain an `lecs::EntityId` as the first parameter; in that case, the EntityId will be sent to the lambda.
     * The component you want to loop over is specified as a template argument, and the lambda can take the component as:
     *  - value <int>
     *  - const value <const int>
     *  - const reference <const int&>
     *  - reference <int&> **Only if the registry is non-const**
     * 
     * It will loop over all entities that have the specified component.
    */
    Logger.Info("foreach_unique_component (With Entity id_):");
    registry.foreach_unique_component<Name>(
        [](lecs::EntityId entityId, const Name& name)
        {
            Logger.Info("    {} -> {}", entityId, name);
        }
    );

    std::cout << std::endl;
    Logger.Info("foreach_unique_component (Without Entity id_):");
    registry.foreach_unique_component<Name>(
        [](const Name& name)
        {
            Logger.Info("    {}", name);
        }
    );

    /****** Add Custom Components ******/
    std::cout << std::endl;
    /**
     * You can add any custom component like any other type. These components do not need to inherit from anything.
    */
    registry.Add<ABigComponent>(alice);
    registry.Add<ASmallComponent>(alice);
    registry.Add<ABigComponent>(bob);

    /****** foreach_components ******/
    std::cout << std::endl;
    /**
     * To ForEach on multiple components (only one included), use foreach_components.
     * Like `foreach_unique_component`, it takes a lambda which can optionally contain an `lecs::EntityId` as the first parameter; in that case, the EntityId will be sent to the lambda.
     * The component you want to loop over is specified as a template argument, and the lambda can take the component as:
     *  - value <int>
     *  - const value <const int>
     *  - const reference <const int&>
     *  - reference <int&> **Only if the registry is non-const**
     * 
     * It will loop over all entities that have **ALL** the specified components.
    */
    Logger.Info("foreach_components (Without Entity id_) on <Name, int>:");
    registry.foreach_components<Name, int>(
        [](const Name& name, const int k)
        {
            Logger.Info("    {} -> {}", name, k);
        }
    );

    std::cout << std::endl;
    /**
     * Here, both Alice and Bob have <int> and <ABigComponent>, so they will both be printed.
    */
    Logger.Info("foreach_components (With Entity id_) on <Name, int, ABigComponent>:");
    Logger.Debug("Both Alice and Bob have <Name>, <int>, and <ABigComponent>");
    registry.foreach_components<Name, int, ABigComponent>(
        [](lecs::EntityId entityId, const Name& name, const int k, const ABigComponent&)
        {
            Logger.Info("    {} ({}) -> {}", name, entityId, k);
        }
    );

    std::cout << std::endl;
    /**
     * Here, only Alice has all three required components: <ASmallComponent>, <int>, <ABigComponent>, so only she will be printed.
     * Bob doesn't have an <ASmallComponent>, so he will not be printed.
    */
    Logger.Info("foreach_components (With Entity id_) on <Name, ASmallComponent, int, ABigComponent>:");
    Logger.Debug("Only Alice has <ASmallComponent>, so Bob is not printed. Alice appears since she has all required components");
    registry.foreach_components<Name, ASmallComponent, int, ABigComponent>(
        [](lecs::EntityId entityId, const Name& name, const ASmallComponent&, const int k, const ABigComponent&)
        {
            Logger.Info("    {} ({}) -> {}", name, entityId, k);
        }
    );

    /****** each_entities_with ******/
    std::cout << std::endl;
    /**
     * each_entities_with will loop over every entity that has all required components (here only <Name>).
    */
    Logger.Info("each_entities_with <Name>:");
    for (lecs::EntityId entityId : registry.each_entities_with<Name>())
    {
        Logger.Info("    {} -> {}", registry.get<Name>(entityId),
            registry.has<ASmallComponent>(entityId) ? "has <ASmallComponent>" : "Does NOT have <ASmallComponent>"
        );
    }

    /****** Views ******/
    std::cout << std::endl;
    Logger.Info("Views");
    
    /**
     * You can create views on components:
     * Here, a view on <int> and <ABigComponent>.
     * It will only be able to view those components; any reference to another component will result in a 'constraint was not satisfied' error.
    */
    auto view = registry.View<Name, int>();

    /**
     * With views, you have access to all previously defined For and Each functions,
     * but this time you cannot reference components that were not present in the constructor.
    */
    Logger.Info("View: Entity with the component <Name>");
    for (lecs::EntityId entityId : view.each_entities_with<Name>())
    {
        Logger.Info("    {} ({})", registry.get<Name>(entityId), entityId);
    }

    lecs::EntityId eve = registry.create_entityid();
    registry.Add<Name>(eve, "Eve");

    std::cout << std::endl;
    Logger.Info("View doesn't need to be recreated; it is always up to date (After adding Eve):");
    for (lecs::EntityId entityId : view.each_entities_with<Name>())
    {
        Logger.Info("    {} ({})", registry.get<Name>(entityId), entityId);
    }

    std::cout << std::endl;
    /**
     * With views, you can use the regular for-each loop on components.
     * And use C++17 structural bindings.
     * Here Eve will not appear since she doesn't have an <int> component.
    */
    Logger.Info("Views: each_components");
    for (const auto& [name, intComponent] : view.each_components<Name, int>())
    {
        Logger.Info("    {} -> {}", name, intComponent);
    }

    registry.Add<int>(eve, 123);

    /**
     * Again, no need to recreate the view; a view is always up to date.
    */
    std::cout << std::endl;
    Logger.Info("View after adding an <int> component to Eve");
    for (const auto& [name, intComponent] : view.each_components<Name, int>())
    {
        Logger.Info("    {} -> {}", name, intComponent);
    }

    /**
     * To destroy an entity, just use destroy_entityid.
    */
    std::cout << std::endl;
    registry.destroy_entityid(bob);

    Logger.Info("View after destroying Bob");
    for (const auto& [name, intComponent] : view.each_components<Name, int>())
    {
        Logger.Info("    {} -> {}", name, intComponent);
    }
}
