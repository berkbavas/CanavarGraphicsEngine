#pragma once

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
