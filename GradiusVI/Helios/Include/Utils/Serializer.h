#pragma once
#include "Core/Core.h"

namespace Helios
{
    class Entity;
    class Scene;

    class Serializer
    {
    public:
        static const char* SERIALIZATION_EXTENSION;

        static std::string Serialize(const rttr::instance& a_obj, bool a_recursively = true);
        static std::string SerializePrefab(const rttr::instance& a_obj, bool a_recursively = true);

    private:
        static void ToJson(const rttr::instance& a_obj2, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer, bool a_recursively = true);
        static bool WriteVariant(const rttr::variant& a_var, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer, bool a_recursively = true);
        static bool WriteAtomicTypes(const rttr::type& a_type, const rttr::variant& a_var, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer);
        static void WriteArray(const rttr::variant_sequential_view& a_view, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer);
        static void WriteMap(const rttr::variant_associative_view& a_view, rapidjson::PrettyWriter<rapidjson::StringBuffer>& a_writer);
        static void RecursiveChildSerialization(std::vector<Entity*>& m_vectorToAdd, const Entity& parent);
    };

    class Deserializer
    {
    public:
        /**
         * \brief Deserialize any (instantiated) object with JSON.
         *
         * \param a_json The json meta string containing the data needed to deserialize.
         * \param a_obj Which object should be deserialized needs to be already instantiated.
         * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
         *
         * \return Whether we could parse the given JSON data.
         **/
        static bool Deserialize(const std::string& a_json, const rttr::instance& a_obj, bool a_recursively = true);
    	
        /**
         * \brief Deserialize any (instantiated) object with JSON.
         *
         * \param a_jsonObj An JSON object containing the meta data.
         * \param a_obj Which object should be deserialized needs to be already instantiated.
         * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
         *
         * \return Whether we could parse the given JSON data.
         **/
        static void Deserialize(const rapidjson::Value& a_jsonObj, const rttr::instance& a_obj, bool a_recursively = true);

    	/**
    	 * \brief Deserialize an instantiated entity from JSON.
    	 *
    	 * \param a_entity Entity to fill with deserialized data.
    	 * \param a_jsonObj JSON object to deserialize with.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 **/
        static void DeserializeEntity(Entity& a_entity, const rapidjson::Value& a_jsonObj, bool a_recursively = true);

    	/**
    	 * \brief Deserialize an instantiated scene from JSON.
    	 *
    	 * \param a_scene scene to fill with deserialized data.
    	 * \param a_jsonMeta JSON string to deserialize with.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 **/
        static bool DeserializeScene(Scene& a_scene, const std::string& a_jsonMeta, bool a_recursively = true);
    	
    	/**
    	 * \brief Deserialize an instantiated scene from JSON.
    	 *
    	 * \param a_scene scene to fill with deserialized data.
    	 * \param a_jsonObj JSON object to deserialize with.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 **/
        static void DeserializeScene(Scene& a_scene, const rapidjson::Value& a_jsonObj, bool a_recursively = true);

    	/**
    	 * \brief Deserialize and create entities from prefab data.
    	 *
    	 * \param a_jsonObj JSON object to deserialize with.
    	 * \param a_prefabPath The path where the prefab data is stored.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 * \param a_setUniqueIds Whether we should set the serialized Unique Id for the entities (off by default since you want the EntityManager create your unique id's).
    	 * \param a_createInstant Whether we should create the Entity prefab instantly or through the queue (while in-game).
    	 *
    	 * \return The root of the prefab or the created prefab entity.
    	 **/
        static Entity* DeserializePrefab(const rapidjson::Value& a_jsonObj, const std::string& a_prefabPath, bool a_recursively = true, bool a_setUniqueIds = false, bool a_createInstant = true);

    	/**
    	 * \brief Get any value from the JSON object.
    	 *
    	 * \param a_jsonObj JSON object to deserialize with.
    	 * 
    	 * \return The value stored inside an rttr::varriant use get_value to get the actual value.
    	 **/
        static rttr::variant GetBasicTypes(const rapidjson::Value& a_jsonObj);

    	/**
    	 * \brief Deserialize an array.
    	 *
    	 * \param a_view The RTTR equivalent of an array type.
    	 * \param a_jsonArrVal The array value that contains our data to deserialize.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 *
    	 * \return The root of the prefab or the created prefab entity.
    	 **/
        static void DeserializeArray(rttr::variant_sequential_view& a_view, const rapidjson::Value& a_jsonArrVal, bool a_recursively = true);
    	
    	/**
    	 * \brief Deserialize an map.
    	 *
    	 * \param a_view The RTTR equivalent of an type type.
    	 * \param a_jsonMapVal The map value that contains our data to deserialize.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 *
    	 * \return The root of the prefab or the created prefab entity.
    	 **/
        static void DeserializeMap(rttr::variant_associative_view& a_view, const rapidjson::Value& a_jsonMapVal, bool a_recursively = true);

    	/**
    	 * \brief Deserialize any property.
    	 *
    	 * \param a_property A property to be deserialized.
    	 * \param a_obj The instance this property belongs to.
    	 * \param a_jsonObj The JSON object containing our data for the property.
    	 * \param a_recursively Whether to go deeper inside the object and deserialize its properties as well.
    	 *
    	 * \return Whether we successfully could deserialize the property.
    	 **/
        static bool DeserializeProperty(rttr::property& a_property, const rttr::instance& a_obj, const rapidjson::Value& a_jsonObj, bool a_recursively = true);
    private:
        static void FromJSON(const rttr::instance& a_obj2, const rapidjson::Value& a_jsonObj, bool a_recursively);

        static Entity* DeserializeEntityScene(const rapidjson::Value& a_jsonObj, bool a_recursively, bool a_setUniqueIds, bool a_createInstant);
        static void RecalculateTransform(const std::vector<Entity*>& a_entities, std::vector<std::pair<EntityID, Entity*>>& a_parents);

        static rttr::variant ExtractValue(const rapidjson::Value::ConstMemberIterator& a_itr, const rttr::type& a_type, bool a_recursively);
    };
}
