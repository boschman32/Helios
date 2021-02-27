#include "hepch.h"
#include "Utils/Serializer.h"

#include "Core/Core.h"
#include "Core/CoreSerializationNames.h"
#include "Core/EntityComponent/EntityManager.h"
#include "Core/ResourceManager.h"

#include "SceneManagement/SceneManager.h"

namespace Helios
{
    const char* Serializer::SERIALIZATION_EXTENSION = ".meta";

    /*
     * =====================================================================================================================================
     * Serialization using JSON
     * source: https://github.com/rttrorg/rttr/blob/master/src/examples/json_serialization/to_json.cpp
     * =====================================================================================================================================
     */
    std::string Serializer::Serialize(const rttr::instance& a_obj, bool a_recursively)
    {
        if (!a_obj.is_valid())
        {
            HE_CORE_WARN("Can't serialize object!");
            return std::string();
        }

        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

        //Convert object data into json data (looks through the object recursively if enabled).
        ToJson(a_obj, writer, a_recursively);
        return sb.GetString();
    }

    std::string Serializer::SerializePrefab(const rttr::instance& a_obj, bool a_recursively)
    {
        if (!a_obj.is_valid())
        {
            HE_CORE_WARN("Can't serialize object!");
            return std::string();
        }

        const auto e = a_obj.try_convert<Entity>();

        if (e == nullptr)
        {
            HE_CORE_WARN("Can't serialize object as it is not a prefab!");
            return std::string();
        }

        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

        if (a_obj.get_type() == rttr::type::get<Entity>())
        {
            //First of all check if we have children, if we don't proceed normally.
            if (e->GetTransform().GetChildrenCount() == 0)
            {
                //We don't have children which is great, serialize normally
                ToJson(a_obj, writer, a_recursively);
            }
            else
            {
                //We have children which makes it a bit trickier, we just have to recursively go though all children and add them to a vector of entities.
                std::vector<Entity*> m_entitiesToSerialize{ e };
                RecursiveChildSerialization(m_entitiesToSerialize, *e);
                //This in essence is a mini scene so we threat it as a scene
                Scene s(S_PREFAB_NAME);
                for (auto ets : m_entitiesToSerialize)
                {
                    s.AddEntity(ets);
                }
                ToJson(s, writer, a_recursively);
            }
        }
        else
        {
            HE_CORE_WARN("This is not an Entity!");
            return std::string();
        }

        return sb.GetString();
    }

    void Serializer::RecursiveChildSerialization(std::vector<Entity*>& m_vectorToAdd, const Entity& a_parent)
    {
        for (auto child : a_parent.GetTransform().GetChildren())
        {
            m_vectorToAdd.push_back(&child->GetOwner());
            if (child->GetChildrenCount() > 0)
            {
                RecursiveChildSerialization(m_vectorToAdd, child->GetOwner());
            }
        }
    }

    void Serializer::ToJson(const rttr::instance& a_obj2, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer, bool a_recursively)
    {
        a_writer.StartObject();
        const rttr::instance obj = a_obj2.get_type().get_raw_type().is_wrapper() ? a_obj2.get_wrapped_instance() : a_obj2;

        //Always add the type to the json file.
        a_writer.String(S_TYPE);
        const std::string typeName = obj.get_derived_type().get_name().to_string();
        a_writer.String(typeName.c_str(), static_cast<rapidjson::SizeType>(typeName.size()));

        const auto propList = obj.get_derived_type().get_properties();
        for (auto& prop : propList)
        {
            if (prop.get_metadata(NO_SERIALIZE))
            {
                continue;
            }

            const rttr::variant propValue = prop.get_value(obj);
            if (!propValue)
            {
                continue; // cannot serialize, because we cannot retrieve the value
            }

            const auto name = prop.get_name();
            a_writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
            if (!WriteVariant(propValue, a_writer, a_recursively))
            {
                HE_CORE_ERROR("Cannot serialize property: {0}", name);
            }
        }

        a_writer.EndObject();
    }

    bool Serializer::WriteVariant(const rttr::variant& a_var, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer, bool a_recursively)
    {
        const auto valueType = a_var.get_type();
        const auto wrappedType = valueType.is_wrapper() ? valueType.get_wrapped_type() : valueType;
        const bool isWrapper = wrappedType != valueType;

        //Is basic type
        if (WriteAtomicTypes(isWrapper ? wrappedType : valueType,
            isWrapper ? a_var.extract_wrapped_value() : a_var, a_writer))
        {

        }
        //Is array
        else if (a_var.is_sequential_container())
        {
            WriteArray(a_var.create_sequential_view(), a_writer);
        }
        //Is map
        else if (a_var.is_associative_container())
        {
            WriteMap(a_var.create_associative_view(), a_writer);
        }
        //If is another object.
        else if (a_recursively)
        {
            const auto childProps = isWrapper ? wrappedType.get_properties() : valueType.get_properties();
            //Has properties so we need to serialize it as well.
            if (!childProps.empty())
            {
                ToJson(a_var, a_writer);
            }
            else
            {
                bool ok = false;
                const auto text = a_var.to_string(&ok);
                if (!ok)
                {
                    a_writer.String(text.c_str());
                    return false;
                }

                a_writer.String(text.c_str());
            }
        }

        return true;
    }

    bool Serializer::WriteAtomicTypes(const rttr::type& a_type, const rttr::variant& a_var, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer)
    {
        if (a_type.is_arithmetic())
        {
            if (a_type == rttr::type::get<bool>())
                a_writer.Bool(a_var.to_bool());
            else if (a_type == rttr::type::get<char>())
                a_writer.Bool(a_var.to_bool());
            else if (a_type == rttr::type::get<int8_t>())
                a_writer.Int(a_var.to_int8());
            else if (a_type == rttr::type::get<int16_t>())
                a_writer.Int(a_var.to_int16());
            else if (a_type == rttr::type::get<int32_t>())
                a_writer.Int(a_var.to_int32());
            else if (a_type == rttr::type::get<int64_t>())
                a_writer.Int64(a_var.to_int64());
            else if (a_type == rttr::type::get<uint8_t>())
                a_writer.Uint(a_var.to_uint8());
            else if (a_type == rttr::type::get<uint16_t>())
                a_writer.Uint(a_var.to_uint16());
            else if (a_type == rttr::type::get<uint32_t>())
                a_writer.Uint(a_var.to_uint32());
            else if (a_type == rttr::type::get<uint64_t>())
                a_writer.Uint64(a_var.to_uint64());
            else if (a_type == rttr::type::get<float>())
                a_writer.Double(a_var.to_double());
            else if (a_type == rttr::type::get<double>())
                a_writer.Double(a_var.to_double());

            return true;
        }

        if (a_type.is_enumeration())
        {
            bool ok = false;
            const auto result = a_var.to_string(&ok);
            if (ok)
            {
                a_writer.String(a_var.to_string().c_str());
            }
            else
            {
                ok = false;
                const auto value = a_var.to_uint64(&ok);
                if (ok)
                    a_writer.Uint64(value);
                else
                    a_writer.Null();
            }

            return true;
        }

        if (a_type == rttr::type::get<std::string>())
        {
            a_writer.String(a_var.to_string().c_str());
            return true;
        }

        return false;
    }

    void Serializer::WriteArray(const rttr::variant_sequential_view& a_view, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer)
    {
        a_writer.StartArray();
        for (const auto& item : a_view)
        {
            if (item.is_sequential_container())
            {
                WriteArray(item.create_sequential_view(), a_writer);
            }
            else
            {
                const rttr::variant wrappedVar = item.extract_wrapped_value();
                const rttr::type valueType = wrappedVar.get_type();
                if (valueType.is_arithmetic() || valueType == rttr::type::get<std::string>() || valueType.is_enumeration())
                {
                    WriteAtomicTypes(valueType, wrappedVar, a_writer);
                }
                else // object
                {
                    ToJson(wrappedVar, a_writer);
                }
            }
        }
        a_writer.EndArray();
    }

    void Serializer::WriteMap(const rttr::variant_associative_view& a_view, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer)
    {
        static const rttr::string_view keyName("key");
        static const rttr::string_view valueName("value");

        a_writer.StartArray();

        if (a_view.is_key_only_type())
        {
            for (const auto& item : a_view)
            {
                WriteVariant(item.first, a_writer);
            }
        }
        else
        {
            for (const auto& item : a_view)
            {
                a_writer.StartObject();
                a_writer.String(keyName.data(), static_cast<rapidjson::SizeType>(keyName.length()), false);

                WriteVariant(item.first, a_writer);

                a_writer.String(valueName.data(), static_cast<rapidjson::SizeType>(valueName.length()), false);

                WriteVariant(item.second, a_writer);

                a_writer.EndObject();
            }
        }

        a_writer.EndArray();
    }

    /*
     * =====================================================================================================================================
     * Deserialization using JSON
     * source: https://github.com/rttrorg/rttr/blob/master/src/examples/json_serialization/from_json.cpp
     * =====================================================================================================================================
     */
    bool Deserializer::Deserialize(const std::string& a_json, const rttr::instance& a_obj, bool recursively)
    {
        rapidjson::Document doc;
        if (doc.Parse(a_json.c_str()).HasParseError())
        {
            HE_CORE_ERROR("Cannot deserialize Entity as it has parsing errors!");
            return false;
        }
        Deserialize(doc, a_obj, recursively);
        return true;
    }

    void Deserializer::Deserialize(const rapidjson::Value& a_jsonObj, const rttr::instance& a_obj, bool recursively)
    {
        FromJSON(a_obj, a_jsonObj, recursively);
    }

    void Deserializer::FromJSON(const rttr::instance& a_obj2, const rapidjson::Value& a_jsonObj, bool a_recursively)
    {
        const rttr::instance obj = a_obj2.get_type().get_raw_type().is_wrapper() ? a_obj2.get_wrapped_instance() : a_obj2;
        const auto propList = obj.get_derived_type().get_properties();

        for (auto prop : propList)
        {
            if (prop.is_readonly() || prop.get_metadata(NO_DESERIALIZE))
            {
                continue;
            }

            if (!DeserializeProperty(prop, obj, a_jsonObj, a_recursively))
            {
                HE_CORE_WARN("[Deserializer] Couldn't deserialize property {0} on object {1}", prop.get_name(), obj.get_type().get_name());
            }
        }
    }

    bool Deserializer::DeserializeProperty(rttr::property& a_property, const rttr::instance& a_obj, const rapidjson::Value& a_jsonObj, bool a_recursively)
    {
        const auto ret = a_jsonObj.FindMember(a_property.get_name().data());
        if (a_property.is_readonly() || ret == a_jsonObj.MemberEnd())
        {
            return false;
        }
        const rttr::type valueType = a_property.get_type().is_wrapper() ? a_property.get_type().get_wrapped_type() : a_property.get_type();

        auto& jsonVal = ret->value;
        switch (jsonVal.GetType())
        {
        case rapidjson::kArrayType:
        {
            rttr::variant var;
            if (valueType.is_sequential_container())
            {
                var = a_property.get_value(a_obj);
                auto view = var.create_sequential_view();
                DeserializeArray(view, jsonVal, a_recursively);
            }
            else if (valueType.is_associative_container())
            {
                var = a_property.get_value(a_obj);
                auto associative_view = var.create_associative_view();
                DeserializeMap(associative_view, jsonVal, a_recursively);
            }

            if (!a_property.set_value(a_obj, var))
            {
                HE_CORE_WARN("[Deserialization] Couldn't set array type for property {0}", a_property.get_name());
            }
            break;
        }
        case rapidjson::kObjectType:
        {
            rttr::variant var = a_property.get_value(a_obj);

            //If the property is a resource we should load it in through the resource manager.
            if (valueType.is_derived_from<Resource>())
            {
                if (!jsonVal.HasMember(S_RESOURCE_ARCHIVE_NAME))
                {
                    if (var != nullptr)
                    {
                        std::shared_ptr<Resource> resource = var.get_value<std::shared_ptr<Resource>>();
                        a_property.set_value(a_obj, ResourceManager::GetInstance().CreateEmptyResource(resource->GetType()));
                        ResourceManager::GetInstance().UnloadResource(std::move(resource));
                    }
                    return true;
                }

                const std::string resourceType = jsonVal.FindMember(S_TYPE)->value.GetString();
                const rapidjson::Value& archiveValue = jsonVal.FindMember(S_RESOURCE_ARCHIVE_NAME)->value;
                const ResourceArchive archive;
                Deserialize(archiveValue, archive);

                //If a resource is already loaded in, unload the previous one (if it is not the same).
                if (var != nullptr && var.get_value<std::shared_ptr<Resource>>()->GetPath() != archive.m_path)
                {
                    std::shared_ptr<Resource> resource = var.get_value<std::shared_ptr<Resource>>();
                    a_property.set_value(a_obj, ResourceManager::GetInstance().CreateEmptyResource(resource->GetType()));
                    ResourceManager::GetInstance().UnloadResource(std::move(resource));
                }

                var = ResourceManager::GetInstance().LoadResourceWithArchive(resourceType, archive);
            }
            else
            {
                if (a_recursively)
                {
                    FromJSON(var, jsonVal, a_recursively);
                }
            }

            if (!a_property.set_value(a_obj, var))
            {
                HE_CORE_WARN("[Deserialization] Couldn't set object value for property {0}", a_property.get_name());
            }
            break;
        }
        default:
        {
            rttr::variant extractedValue = GetBasicTypes(jsonVal);
            if (extractedValue.convert(valueType))
            {
                //REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
                if (!a_property.set_value(a_obj, extractedValue))
                {
                    HE_CORE_WARN("[Deserialization] Couldn't set basic value for property {0}", a_property.get_name());
                }
            }
        }
        }
        return true;
    }


    rttr::variant Deserializer::GetBasicTypes(const rapidjson::Value& a_jsonObj)
    {
        switch (a_jsonObj.GetType())
        {
        case rapidjson::kStringType:
        {
            return std::string(a_jsonObj.GetString());
        }
        case rapidjson::kNullType:     break;
        case rapidjson::kFalseType:
        case rapidjson::kTrueType:
        {
            return a_jsonObj.GetBool();
        }
        case rapidjson::kNumberType:
        {
            if (a_jsonObj.IsInt())
                return a_jsonObj.GetInt();
            if (a_jsonObj.IsFloat())
                return a_jsonObj.GetFloat();
            if (a_jsonObj.IsDouble())
                return a_jsonObj.GetDouble();
            if (a_jsonObj.IsUint())
                return a_jsonObj.GetUint();
            if (a_jsonObj.IsInt64())
                return a_jsonObj.GetInt64();
            if (a_jsonObj.IsUint64())
                return a_jsonObj.GetUint64();
            break;
        }
        // we handle only the basic types here
        case rapidjson::kObjectType:
        case rapidjson::kArrayType: return rttr::variant();
        }

        return rttr::variant();
    }

    void Deserializer::DeserializeArray(rttr::variant_sequential_view& a_view, const rapidjson::Value& a_jsonArrVal, bool a_recursively)
    {
        a_view.set_size(a_jsonArrVal.Size());
        const rttr::type arrayValueType = a_view.get_rank_type(1);

        for (rapidjson::SizeType i = 0; i < a_jsonArrVal.Size(); ++i)
        {
            auto& jsonIndexVal = a_jsonArrVal[i];
            if (jsonIndexVal.IsArray())
            {
                auto subArray = a_view.get_value(i).create_sequential_view();
                DeserializeArray(subArray, jsonIndexVal, a_recursively);
            }
            else if (jsonIndexVal.IsObject())
            {
                const rttr::variant varTmp = a_view.get_value(i);
                const rttr::variant wrappedVar = varTmp.extract_wrapped_value();
                if (a_recursively)
                {
                    FromJSON(wrappedVar, jsonIndexVal, a_recursively);
                }

                a_view.set_value(i, wrappedVar);
            }
            else
            {
                rttr::variant extractedValue = GetBasicTypes(jsonIndexVal);
                if (extractedValue.convert(arrayValueType))
                    a_view.set_value(i, extractedValue);
            }
        }
    }

    void Deserializer::DeserializeMap(rttr::variant_associative_view& a_view, const rapidjson::Value& a_jsonMapVal, bool a_recursively)
    {
        for (rapidjson::SizeType i = 0; i < a_jsonMapVal.Size(); ++i)
        {
            auto& jsonIndexVal = a_jsonMapVal[i];
            if (jsonIndexVal.IsObject()) // a key-value associative view
            {
                const auto keyItr = jsonIndexVal.FindMember("key");
                const auto valueItr = jsonIndexVal.FindMember("value");

                if (keyItr != jsonIndexVal.MemberEnd() &&
                    valueItr != jsonIndexVal.MemberEnd())
                {
                    const auto key = ExtractValue(keyItr, a_view.get_key_type(), a_recursively);
                    const auto value = ExtractValue(valueItr, a_view.get_value_type(), a_recursively);
                    if (key && value)
                    {
                        const auto inserted = a_view.insert(key, value);
                        if (!inserted.second)
                        {
                            HE_CORE_ASSERT(false, "Can't insert data!");
                        }
                    }
                }
            }
            else // a key-only associative view
            {
                rttr::variant extractedVal = GetBasicTypes(jsonIndexVal);
                if (extractedVal && extractedVal.convert(a_view.get_key_type()))
                {
                    const auto inserted = a_view.insert(extractedVal);
                    if (!inserted.second)
                    {
                        HE_CORE_ASSERT(false, "Can't insert data!");
                    }
                }
            }
        }
    }

    rttr::variant Deserializer::ExtractValue(const rapidjson::Value::ConstMemberIterator& a_itr, const rttr::type& a_type, bool a_recursively)
    {
        const auto& jsonValue = a_itr->value;
        rttr::variant extractedVal = GetBasicTypes(jsonValue);
        const bool canConvert = extractedVal.convert(a_type);
        if (!canConvert)
        {
            if (jsonValue.IsObject())
            {
                rttr::constructor ctor = a_type.get_constructor();
                for (auto& item : a_type.get_constructors())
                {
                    if (item.get_instantiated_type() == a_type)
                    {
                        ctor = item;
                    }
                }
                extractedVal = ctor.invoke();

                if (a_recursively)
                {
                    FromJSON(extractedVal, jsonValue, a_recursively);
                }
            }
        }

        return extractedVal;
    }

    void Deserializer::DeserializeEntity(Entity& a_entity, const rapidjson::Value& a_jsonObj, bool a_recursively)
    {
        const bool hasPrefab = a_jsonObj.FindMember(S_ENTITY_PROP_HAS_PREFAB)->value.GetBool();
        const rttr::instance& entityInst = a_entity;
        if (hasPrefab)
        {
            entityInst.get_type().set_property_value(S_ENTITY_PROP_HAS_PREFAB, entityInst, hasPrefab);
            const std::string prefabPath = a_jsonObj.FindMember(S_ENTITY_PROP_PREFAB_PATH)->value.GetString();
            entityInst.get_type().set_property_value(S_ENTITY_PROP_PREFAB_PATH, entityInst, prefabPath);
        }

        const auto serializedComponents = a_jsonObj.FindMember(S_ENTITY_PROP_COMPONENTS)->value.GetArray();

        //Go through all the components and create/update and deserialize them.
        for (auto comp = serializedComponents.Begin(); comp != serializedComponents.End(); ++comp)
        {
            const std::string strType = comp->FindMember(S_TYPE)->value.GetString();

            const ComponentID compId
            {
                std::size_t(comp->FindMember(S_COMPONENT_PROP_ID)->value.GetObjectW().FindMember(S_COMPONENTID_PROP_ID)->value.GetInt())
            };

            const Component* existingComponent = a_entity.GetComponentByID(compId);
            if (existingComponent != nullptr)
            {
                //This component is already on the entity so we can just update the data.
                Deserialize(*comp, *existingComponent, a_recursively);
            }
            else
            {
                //Create the correct component through reflection.
                const rttr::type classType = rttr::type::get_by_name(strType);
                HE_CORE_ASSERT(classType.is_valid(), "Component class is not registered!");
                if (!classType.get_metadata(NO_DESERIALIZE))
                {
                    const rttr::variant obj = classType.create({ a_entity, compId });

                    //Deserialize the component with the meta data.
                    Deserialize(*comp, obj, a_recursively);

                    //Add the deserialized component to the entity.
                    a_entity.AddComponent(std::unique_ptr<Component>(obj.get_value<Component*>()));
                }
            }
        }

        //Disable or enable if needed.
        bool isEnabled = a_jsonObj.FindMember(S_ENTITY_PROP_ENABLED)->value.GetBool();
        a_entity.get_type().set_property_value(S_ENTITY_PROP_ENABLED, entityInst, isEnabled);
    }

    Entity* Deserializer::DeserializePrefab(const rapidjson::Value& a_jsonObj, const std::string& a_prefabPath, bool a_recursively, bool a_setUniqueIds, bool a_createInstant)
    {
        Entity* prefabEntity;
        //This is just one entity with no children so just deserialize it.
        const std::string type = a_jsonObj.FindMember(S_TYPE)->value.GetString();
        if (type == S_ENTITY_NAME)
        {
            const std::string entityName = a_jsonObj.FindMember(S_ENTITY_PROP_NAME)->value.GetString();
            const bool hasCloneInName = entityName.find("(Clone)") != std::string::npos;

            if (a_createInstant)
            {
                prefabEntity = EntityManager::GetInstance().CreateEntityDirectly(entityName + (hasCloneInName ? "" : " (Clone)"),
                    SceneManager::GetInstance().GetActiveScene(), false);
            }
            else
            {
                prefabEntity = EntityManager::GetInstance().CreateEntity(entityName + (hasCloneInName ? "" : " (Clone)"), false);
            }
            DeserializeEntity(*prefabEntity, a_jsonObj, a_recursively);
        }
        else
        {
            //This entity has children, threat it as a scene, deserialize everything than set parents.
            prefabEntity = DeserializeEntityScene(a_jsonObj, a_recursively, a_setUniqueIds, a_createInstant);
        }

        prefabEntity->m_hasPrefab = true;
        prefabEntity->m_prefabPath = a_prefabPath;
        return prefabEntity;
    }

    Entity* Deserializer::DeserializeEntityScene(const rapidjson::Value& a_jsonObj, bool a_recursively, bool a_setUniqueIds, bool a_createInstant)
    {
        std::vector < std::pair<EntityID, Entity*>> changedParents;
        std::vector<Entity*> createdEntities;
        Entity* root = nullptr;

        const auto entities = a_jsonObj.FindMember(S_SCENE_PROP_ENTITIES)->value.GetArray();
        for (rapidjson::SizeType i = 0; i < entities.Size(); ++i)
        {
            const auto& entityJSON = entities[i];
            //Find the name of the entity then create one with it.
            std::string entityName = entityJSON.FindMember(S_ENTITY_PROP_NAME)->value.GetString();
            const auto oldParent = EntityID(entityJSON.FindMember(S_ENTITY_PROP_ENTITY_ID)->value.FindMember(S_ENTITYID_PROP_ID)->value.GetInt());

            //Add (Clone) to the name of the root if it not already has it.
            if (i == 0)
            {
                const bool hasCloneInName = entityName.find("(Clone)") != std::string::npos;
                entityName += (hasCloneInName ? "" : " (Clone)");
            }

            Entity* createdEntity;
            if (a_createInstant)
            {
                createdEntity = EntityManager::GetInstance().CreateEntityDirectly(entityName, SceneManager::GetInstance().GetActiveScene(), false);
            }
            else
            {
                createdEntity = EntityManager::GetInstance().CreateEntity(entityName, false);
            }

            //The first entity is the root.
            if (i == 0)
            {
                root = createdEntity;
            }

            //If we want to use the unique id stored inside the json, (off by default since normally you want the unique id to be created through the EntityManager).
            if (a_setUniqueIds)
            {
                const int uniqueId = entityJSON.FindMember(S_ENTITY_PROP_ENTITY_ID)->value.GetObjectW().FindMember(S_ENTITYID_PROP_ID)->value.GetInt();
                const EntityID id(uniqueId);
                createdEntity->SetEntityID(id);
            }

            changedParents.emplace_back(std::make_pair(oldParent, createdEntity));
            createdEntities.push_back(createdEntity);

            DeserializeEntity(*createdEntity, entityJSON, a_recursively);
        }

        RecalculateTransform(createdEntities, changedParents);

        return root;
    }

    void Deserializer::RecalculateTransform(const std::vector<Entity*>& a_entities, std::vector<std::pair<EntityID, Entity*>>& a_parents)
    {
        //Basically getting all old parent IDs and linking the new ones with it
        //Once we are done with adding all new entities we have new ID's and we basically get each transform and check if they have a parent, if they do, look in our vector and
        //Get the new ID based on the old ID.
        for (auto& e : a_entities)
        {
            auto& t = e->GetTransform();
            if (t.GetChildrenCount() > 0)
            {
                t.ClearChildren();
            }
            if (t.m_parentOwnerId != -1)
            {
                const auto savedId = EntityID(t.m_parentOwnerId);
                //Check which current id corresponds to the saved id.
                for (auto& p : a_parents)
                {
                    if (p.first == savedId)
                    {
                        t.SetParent(&p.second->GetTransform());
                        break;
                    }
                }
            }
        }
    }

    bool Deserializer::DeserializeScene(Scene& a_scene, const std::string& a_jsonMeta, bool a_recursively)
    {
        rapidjson::Document doc;

        if (doc.Parse(a_jsonMeta.c_str()).HasParseError())
        {
            HE_CORE_ERROR("Cannot deserialize Entity as it has parsing errors!");
            return false;
        }

        DeserializeScene(a_scene, doc, a_recursively);

        return true;
    }

    void Deserializer::DeserializeScene(Scene& a_scene, const rapidjson::Value& a_jsonObj, bool a_recursively)
    {
        std::vector < std::pair<EntityID, Entity*>> changedParents;
        std::vector<Entity*> createdEntities;
        Deserialize(a_jsonObj, a_scene, false);

        const auto entities = a_jsonObj.FindMember(S_SCENE_PROP_ENTITIES)->value.GetArray();
        for (rapidjson::SizeType i = 0; i < entities.Size(); ++i)
        {
            auto& entityJSON = entities[i];
            //Find the name of the entity then create one with it.
            const std::string entityName = entityJSON.FindMember(S_ENTITY_PROP_NAME)->value.GetString();

            const auto oldParent = EntityID(entityJSON.FindMember(S_ENTITY_PROP_ENTITY_ID)->value.FindMember(S_ENTITYID_PROP_ID)->value.GetInt());
            Entity* createdEntity = EntityManager::GetInstance().CreateEntityDirectly(entityName, a_scene, false);

            createdEntities.push_back(createdEntity);
            changedParents.emplace_back(std::make_pair(oldParent, createdEntity));

            DeserializeEntity(*createdEntity, entityJSON, a_recursively);
        }

        RecalculateTransform(createdEntities, changedParents);

        a_scene.SetLoaded(true);
        a_scene.SetValid(true);
    }

}
