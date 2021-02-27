#pragma once
#if HE_DEBUG
#define HE_ENABLE_ASSERT
#endif

#ifdef HE_ENABLE_ASSERT
#define ASSERT(assertion, ...) { if(!(assertion)) { LOG_CRITICAL("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define HE_CORE_ASSERT(assertion, ...) { if(!(assertion)) { HE_CORE_CRITICAL("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define ASSERT(x, ...)
#define HE_CORE_ASSERT(x, ...)
#endif

#define RTTR_REGISTER_CLASS(type, name, ...)	rttr::registration::class_<type>(name).constructor<__VA_ARGS__>()
#define RTTR_REGISTER_COMPONENT(type, name, ...) RTTR_REGISTER_CLASS(type, name, __VA_ARGS__)(rttr::policy::ctor::as_raw_ptr)
#define RTTR_REGISTER_ENUM(type, name, ...) rttr::registration::enumeration<type>(name).operator()(__VA_ARGS__)
#define RTTR_PROPERTY(name, value) .property(name, &value)
#define RTTR_PROPERTY_GET_SET(name, get, set) .property(name, &get, &set)
#define RTTR_PROPERTY_GET_SET_OVERLOAD(name, get, set) .property(name, get, set)
#define RTTR_PROPERTY_READ_ONLY(name,refrence) .property_readonly(name, &refrence)
#define RTTR_SELECT_OVERLOAD(returnType, func, ...) rttr::select_overload<returnType(__VA_ARGS__)>(&func)

template<typename T, typename IdType = int>
class TypeSafeId
{
	RTTR_ENABLE()
public:
	explicit TypeSafeId(IdType a_id = IdType())
		: m_id(a_id) {}
    virtual ~TypeSafeId() = default;

	IdType Value() const noexcept { return m_id; }

	bool operator<(TypeSafeId<T, IdType> a_rhs) const noexcept { return m_id < a_rhs.m_id; }
	bool operator==(TypeSafeId<T, IdType> a_rhs) const noexcept { return m_id == a_rhs.m_id; }
	bool operator!=(TypeSafeId<T, IdType> a_rhs) const noexcept { return m_id != a_rhs.m_id; }
private:
	IdType m_id;

	RTTR_REGISTRATION_FRIEND;
};

template<typename T, typename IdType>
std::ostream& operator<<(std::ostream& a_os,
	TypeSafeId<T, IdType> a_identifier) {
	return a_os << a_identifier.Value();
}

//Core Id's
namespace Helios
{
	class Entity;
	using EntityID = TypeSafeId<Entity, std::uint64_t>;

	class Component;
	using ComponentID = TypeSafeId<Component, std::size_t>;

	class Resource;
	using ResourceId = TypeSafeId<Resource, std::uint64_t>;
}

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Mat3 = glm::mat3;
using Quaternion = glm::quat;