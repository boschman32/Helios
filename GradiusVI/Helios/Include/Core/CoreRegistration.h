#pragma once
#include "Core/Core.h"
#include "Core/CoreSerializationNames.h"
#include "Core/EntityComponent/Component.h"
#include "Components/TestComponent.h"
#include "Components/Transform.h"
#include "Components/Colliders/CircleCollider.h"
#include "Components/Colliders/PolygonCollider.h"
#include "Components/Colliders/QuadCollider.h"
#include "Components/AudioSource.h"
#include "Components/LuaComponent.h"
#include "Components/MeshRenderer.h"
#include "Components/PrimitiveShapeComponent.h"
#include "Components/UIComponent.h"

RTTR_REGISTRATION
{
RTTR_REGISTER_CLASS(glm::vec2, "glmvec2")
RTTR_PROPERTY("x", glm::vec2::x)
RTTR_PROPERTY("y", glm::vec2::y);

RTTR_REGISTER_CLASS(glm::vec3, "glmvec3")
RTTR_PROPERTY("x", glm::vec3::x)
RTTR_PROPERTY("y", glm::vec3::y)
RTTR_PROPERTY("z", glm::vec3::z);

RTTR_REGISTER_CLASS(glm::quat, "glmquat")
RTTR_PROPERTY("x", glm::quat::x)
RTTR_PROPERTY("y", glm::quat::y)
RTTR_PROPERTY("z", glm::quat::z)
RTTR_PROPERTY("w", glm::quat::w);

//Primitive Shapes
RTTR_REGISTER_ENUM(Helios::EShapeType, "Shape Type",
	rttr::value<Helios::EShapeType>("Plane", Helios::EShapeType::EShapeType_Plane),
	rttr::value<Helios::EShapeType>("Box", Helios::EShapeType::EShapeType_Box),
	rttr::value<Helios::EShapeType>("Sphere", Helios::EShapeType::EShapeType_Sphere));

//TEMP Test component
 rttr::registration::class_<Helios::TestComponent>("TestComponent")
.constructor<Helios::Entity&, Helios::ComponentID>()
(
	rttr::policy::ctor::as_raw_ptr
)
.property("myInt", &Helios::TestComponent::myInt)
.property("myBool", &Helios::TestComponent::myBool)
.property("myFloat", &Helios::TestComponent::myFloat)
.property("myStrings", &Helios::TestComponent::myString)
.property("My Prefab", &Helios::TestComponent::myPrefab)
.property("My Array", &Helios::TestComponent::myArray)
.property("My Array Class", &Helios::TestComponent::myArrayClasses);

rttr::registration::class_<Helios::TestClass>("TestClass")
.constructor<int>()
.property("X", &Helios::TestClass::x)
.property("Y", &Helios::TestClass::y)
.property("Z", &Helios::TestClass::z)
.property("W", &Helios::TestClass::w);

//Component & Scriptable Component
RTTR_REGISTER_CLASS(Helios::Component, "Component", Helios::Entity&, Helios::ComponentID);
RTTR_REGISTER_CLASS(Helios::ScriptableComponent, "Scriptable Component", Helios::Entity&, Helios::ComponentID);

//Register Transform
RTTR_REGISTER_COMPONENT(Helios::Transform, S_TRANSFORM_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_TRANSFORM_PROP_LOCAL_POS, Helios::Transform::m_localPosition)
RTTR_PROPERTY(S_TRANSFORM_PROP_LOCAL_ROTATION, Helios::Transform::m_localRotation)
RTTR_PROPERTY_GET_SET_OVERLOAD(S_TRANSFORM_PROP_POS, &Helios::Transform::GetPosition,
	RTTR_SELECT_OVERLOAD(void, Helios::Transform::SetPosition, Vec3))(SET_NO_SERIALIZE, SET_NO_DESERIALIZE)
RTTR_PROPERTY_GET_SET_OVERLOAD(S_TRANSFORM_PROP_ROTATION, &Helios::Transform::GetRotation, 
	RTTR_SELECT_OVERLOAD(void, Helios::Transform::SetRotation, Quaternion))(SET_NO_SERIALIZE, SET_NO_DESERIALIZE)
RTTR_PROPERTY(S_TRANSFORM_PROP_SCALE, Helios::Transform::m_localScale)
RTTR_PROPERTY(S_TRANSFORM_PROP_PARENT, Helios::Transform::m_parentOwnerId)(SET_HIDE_FROM_INSPECTOR);
	
//Register Component
RTTR_REGISTER_COMPONENT(Helios::Component, S_COMPONENT_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_COMPONENT_PROP_ID, Helios::Component::m_id)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY_READ_ONLY(S_COMPONENT_PROP_OWNER_ID, Helios::Component::m_ownerId)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY(S_COMPONENT_PROP_ENABLED, Helios::Component::m_isEnabled)(SET_HIDE_FROM_INSPECTOR);

//Register ComponentID
RTTR_REGISTER_COMPONENT(Helios::ComponentID, S_COMPONENTID_PROP_ID)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY(S_COMPONENTID_PROP_ID, Helios::ComponentID::m_id);

//Register Collider
RTTR_REGISTER_COMPONENT(Helios::Collider, S_COLLIDER_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_COLLIDER_PROP_VISIBLE, Helios::Collider::m_isVisible);

//Register Circle Collider
RTTR_REGISTER_COMPONENT(Helios::CircleCollider, S_CIRCLE_COLLIDER_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_CIRCLE_COLLIDER_PROP_RADIUS, Helios::CircleCollider::m_radius)
RTTR_PROPERTY(S_CIRCLE_COLLIDER_PROP_OFFSET, Helios::CircleCollider::m_offset);

//Register Polygon Collider
RTTR_REGISTER_COMPONENT(Helios::PolygonCollider, S_POLYGON_COLLIDER_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_POLYGON_COLLIDER_PROP_SIZE, Helios::PolygonCollider::m_size)
RTTR_PROPERTY(S_POLYGON_COLLIDER_PROP_OFFSET, Helios::PolygonCollider::m_offset)
RTTR_PROPERTY(S_POLYGON_COLLIDER_PROP_OFFSET_ROTATION, Helios::PolygonCollider::m_offsetRotation);

//Register Quad Collider
RTTR_REGISTER_COMPONENT(Helios::QuadCollider, S_QUAD_COLLIDER_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_QUAD_COLLIDER_PROP_SIZE, Helios::QuadCollider::m_size)
RTTR_PROPERTY(S_QUAD_COLLIDER_PROP_OFFSET, Helios::QuadCollider::m_offset)
RTTR_PROPERTY(S_QUAD_COLLIDER_PROP_OFFSET_ROTATION, Helios::QuadCollider::m_offsetRotation);

//Register Audio Asset
RTTR_REGISTER_COMPONENT(Helios::AudioAsset, S_AUDIO_ASSET_NAME)
RTTR_PROPERTY(S_AUDIO_ASSET_PROP_PATH, Helios::AudioAsset::m_filePath);

//Audio Source
RTTR_REGISTER_COMPONENT(Helios::AudioSource, S_AUDIO_SOURCE_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_AUDIO_SOURCE_PROP_LOOPING, Helios::AudioSource::m_looping)
RTTR_PROPERTY(S_AUDIO_SOURCE_PROP_STREAMING, Helios::AudioSource::m_streaming)
RTTR_PROPERTY(S_AUDIO_SOURCE_PROP_ASSET, Helios::AudioSource::m_asset)
RTTR_PROPERTY(S_AUDIO_SOURCE_PROP_OFFSET, Helios::AudioSource::m_decibelOffset);

//LuaComponent
RTTR_REGISTER_COMPONENT(Helios::LuaComponent, S_LUA_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_LUA_PROP_PATH, Helios::LuaComponent::m_filePath);

//Resource
RTTR_REGISTER_CLASS(Helios::Resource, S_RESOURCE_NAME, Helios::ResourceType)
RTTR_PROPERTY_READ_ONLY(S_RESOURCE_ARCHIVE_NAME, Helios::Resource::GetResourceArchive)(SET_HIDE_FROM_INSPECTOR);

//ResourceArchive
RTTR_REGISTER_CLASS(Helios::ResourceArchive, S_RESOURCE_ARCHIVE_NAME, Helios::ResourceType, std::string, std::string)
RTTR_PROPERTY(S_RESOURCE_ARCHIVE_PROP_TYPE, Helios::ResourceArchive::m_type)
RTTR_PROPERTY(S_RESOURCE_ARCHIVE_PROP_FILENAME, Helios::ResourceArchive::m_fileName)
RTTR_PROPERTY(S_RESOURCE_ARCHIVE_PROP_PATH, Helios::ResourceArchive::m_path);

RTTR_REGISTER_CLASS(Helios::Prefab, S_PREFAB_NAME);
rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<Helios::Prefab>>();


RTTR_REGISTER_CLASS(Helios::MeshAsset, "Mesh Asset")
RTTR_PROPERTY("Filepath",Helios::MeshAsset::m_filepath);

//DX12 Texture
RTTR_REGISTER_CLASS(Helios::DX12Texture, S_DX12TEXTURE_NAME);
rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<Helios::DX12Texture>>();

//Register meshrenderercomponent
RTTR_REGISTER_CLASS(Helios::MaterialAsset, "Material Asset")
RTTR_PROPERTY("Filepath", Helios::MaterialAsset::m_filepath);
	
RTTR_REGISTER_COMPONENT(Helios::MeshRenderer, S_MESH_RENDERER_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY(S_MESH_RENDERER_PROP_MESH, Helios::MeshRenderer::m_mesh)
RTTR_PROPERTY(S_MESH_RENDERER_PROP_MATERIAL, Helios::MeshRenderer::m_material)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY_GET_SET(S_MESH_RENDERER_PROP_TEXTURE_ARCHIVE, Helios::MeshRenderer::GetTextureArchive, Helios::MeshRenderer::LoadTextureFromArchive)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY_GET_SET(S_MESH_RENDERER_PROP_TEXTURE, Helios::MeshRenderer::GetTexture, Helios::MeshRenderer::SetTexture)(SET_NO_SERIALIZE, SET_NO_DESERIALIZE);

//register primitive renderer enum and component
RTTR_REGISTER_ENUM(Helios::EShapeType, "EShapeType",
    rttr::value<Helios::EShapeType>("Plane", Helios::EShapeType::EShapeType_Plane),
    rttr::value<Helios::EShapeType>("Sphere", Helios::EShapeType::EShapeType_Sphere),
    rttr::value<Helios::EShapeType>("Box", Helios::EShapeType::EShapeType_Box));
//Primitive Shape
RTTR_REGISTER_COMPONENT(Helios::PrimitiveShape, S_PRIMITIVE_SHAPE_COMPONENT_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY_GET_SET(S_PRIMITIVE_SHAPE_PROP_SHAPE, Helios::PrimitiveShape::GetCurrentShapeType, Helios::PrimitiveShape::SetCurrentShapeType)
RTTR_PROPERTY_GET_SET(S_PRIMITIVE_SHAPE_PROP_TEXTURE_ARCHIVE, Helios::PrimitiveShape::GetTextureArchive, Helios::PrimitiveShape::LoadTextureFromArchive)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY_GET_SET(S_PRIMITIVE_SHAPE_PROP_TEXTURE, Helios::PrimitiveShape::GetTexture, Helios::PrimitiveShape::SetTexture)(SET_NO_SERIALIZE, SET_NO_DESERIALIZE);

RTTR_REGISTER_COMPONENT(Helios::UIComponent, S_UI_COMPONENT_NAME, Helios::Entity&, Helios::ComponentID)
RTTR_PROPERTY_GET_SET(S_UI_COMPONENT_PROP_TEXTURE, Helios::UIComponent::GetTexture, Helios::UIComponent::SetTexture)(SET_NO_SERIALIZE, SET_NO_DESERIALIZE)
RTTR_PROPERTY_GET_SET(S_UI_COMPONENT_ARCHIVE_NAME, Helios::UIComponent::GetTextureArchive, Helios::UIComponent::LoadTextureFromArchive)(SET_HIDE_FROM_INSPECTOR)
RTTR_PROPERTY(S_UI_COMPONENT_PROP_POSITION, Helios::UIComponent::m_position)
RTTR_PROPERTY(S_UI_COMPONENT_PROP_ANCHOR_POINT, Helios::UIComponent::m_anchorpoint)
RTTR_PROPERTY(S_UI_COMPONENT_PROP_SCALE, Helios::UIComponent::m_scale)
RTTR_PROPERTY(S_UI_COMPONENT_PROP_ROTATION, Helios::UIComponent::m_rotation);

}

