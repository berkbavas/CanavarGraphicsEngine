#pragma once

#include <string>

// Define member
#define DEFINE_MEMBER(TYPE, NAME, ...) \
  protected: \
    TYPE m##NAME{ __VA_ARGS__ }; \
\
  public: \
    inline const TYPE& Get##NAME(void) const \
    { \
        return m##NAME; \
    } \
\
  public: \
    inline TYPE& Get##NAME##_NonConst(void) \
    { \
        return m##NAME; \
    } \
\
  public: \
    inline void Set##NAME(const TYPE& var) \
    { \
        m##NAME = var; \
    }

// Define member constant
#define DEFINE_MEMBER_CONST(TYPE, NAME, ...) \
  protected: \
    TYPE m##NAME{ __VA_ARGS__ }; \
\
  public: \
    inline const TYPE& Get##NAME(void) const \
    { \
        return m##NAME; \
    }

// Define member pointer
#define DEFINE_MEMBER_PTR(TYPE, NAME) \
  protected: \
    TYPE* m##NAME{ nullptr }; \
\
  public: \
    inline TYPE* Get##NAME(void) const \
    { \
        return m##NAME; \
    } \
\
  public: \
    inline void Set##NAME(TYPE* var) \
    { \
        m##NAME = var; \
    }

// Define member pointer constant
#define DEFINE_MEMBER_PTR_CONST(TYPE, NAME) \
  protected: \
    TYPE* m##NAME{ nullptr }; \
\
  public: \
    inline TYPE* Get##NAME(void) const \
    { \
        return m##NAME; \
    }

// Define getter
#define DEFINE_GETTER(Type, Name) \
    inline const Type& Get##Name(void) const \
    { \
        return m##Name; \
    }

// Disable copy
#define DISABLE_COPY(CLASS_NAME) \
    CLASS_NAME(const CLASS_NAME&) = delete; \
    CLASS_NAME& operator=(const CLASS_NAME&) = delete

#define DEFINE_MESH_TYPE_ID(TYPE_ID) \
    static constexpr int MESH_TYPE_ID \
    { \
        TYPE_ID \
    }

#define REGISTER_OBJECT_TYPE(TYPE) \
  public: \
    QString GetNodeType() const override \
    { \
        return #TYPE; \
    } \
    static constexpr const char* NODE_TYPE{ #TYPE }; \
    inline static const Canavar::Engine::ObjectFactory FACTORY = Canavar::Engine::Object::RegisterObjectFactory<TYPE>()

#define REGISTER_NODE_TYPE(TYPE) \
  public: \
    QString GetNodeType() const override \
    { \
        return #TYPE; \
    } \
    static constexpr const char* NODE_TYPE \
    { \
        #TYPE \
    }