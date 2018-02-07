/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#ifndef YTE_Actions_Action_hpp
#define YTE_Actions_Action_hpp
#include <functional>

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
#define YTENewAction(name) \
  class name : public Action_CRTP<name> \
  { \
  public: \
  name(float& aValue, float aFinal, float aDur) \
  : Action_CRTP(aDur), value(aValue), b(aValue), c(aFinal - b) { } \
  float& value; \
  float b; \
  float c; \
  void Init() override \
  { \
    if (b != value) { \
      float f = b + c;  \
      b = value; \
      c = f - b; \
    } \
  } \
  void operator()(); \
  }; \
  inline void name::operator()()

  class Action
  {
  public:
    friend class ActionGroup;
    explicit Action(float aDur);
    float Increment(float dt);
    bool IsDone() const;
    virtual void Init();
    virtual Action * Clone() const;
    virtual void operator() ();
    virtual ~Action();
  protected:
    float d;
    float t;
    float Time;
  };

  template <typename T>
  class Action_CRTP : public Action
  {
  public:
    Action_CRTP(float aDur) : Action(aDur)
    {
      static_assert(std::is_base_of<Action_CRTP, T>::value,
        "Action_CRTP is for curiously recurring template pattern use only");
    }
    Action * Clone() const { return new T(*reinterpret_cast<T*>(this)); }
  };

  class Action_Callback : public Action_CRTP<Action_Callback>
  {
  public:
    Action_Callback(std::function<void(void)> aCallback);
    void operator() () override;
  private:
    bool mHasCalled;
    std::function<void(void)> mCallback;
  };

  namespace Back
  {
    YTENewAction(easeIn)
    {
      float s = static_cast<float>(1.70158f);
      t /= d;
      float postFix = t;
      value = c * (postFix)*t*((s + 1)*t - s) + b;
    }
    YTENewAction(easeOut)
    {
      float s = static_cast<float>(1.70158f);
      value = c * ((t = t / d - 1)*t*((s + 1)*t + s) + 1) + b;
    }
    YTENewAction(easeInOut)
    {
      float s = static_cast<float>(1.70158f);
      if ((t /= d / 2) < 1)
      {
        value = c / 2 * (t*t*(((s *= (1.525f)) + 1)*t - s)) + b;
      }
      else
      {
        float postFix = t -= 2;
        value = c / 2 * ((postFix)*t*(((s *= (1.525f)) + 1)*t + s) + 2) + b;
      }

    }
  }

  namespace Bounce
  {
    inline float eOut(float t, float b, float c, float d)
    {
      if ((t /= d) < (1 / 2.75f)) {
        return c * (7.5625f*t*t) + b;
      }
      else if (t < (2 / 2.75f)) {
        float postFix = t -= (1.5f / 2.75f);
        return c * (7.5625f*(postFix)*t + .75f) + b;
      }
      else if (t < (2.5 / 2.75)) {
        float postFix = t -= (2.25f / 2.75f);
        return c * (7.5625f*(postFix)*t + .9375f) + b;
      }
      else {
        float postFix = t -= (2.625f / 2.75f);
        return c * (7.5625f*(postFix)*t + .984375f) + b;
      }
    }

    inline float eIn(float t, float b, float c, float d)
    {
      return c - eOut(d - t, 0, c, d) + b;
    }
    YTENewAction(easeIn)
    {
      value = eIn(t, b, c, d);
    }
    YTENewAction(easeOut)
    {
      value = eOut(t, b, c, d);
    }

    YTENewAction(easeInOut)
    {
      if (t < d / 2)
      {
        value = eIn(t * 2, 0, c, d) * .5f + b;
      }
      else
      {
        value = eOut(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
      }
    }
  }

  namespace Circ
  {
    YTENewAction(easeIn)
    {
      value = -c * (sqrt(1 - (t /= d)*t) - 1) + b;
    }
    YTENewAction(easeOut)
    {
      value = c * sqrt(1 - (t = t / d - 1)*t) + b;
    }
    YTENewAction(easeInOut)
    {
      if ((t /= d / 2) < 1)
      {
        value = -c / 2 * (sqrt(1 - t * t) - 1) + b;
      }
      else
      {
        value = c / 2 * (sqrt(1 - t * (t -= 2)) + 1) + b;
      }
    }
  }

  namespace Cubic
  {
    YTENewAction(easeIn)
    {
      value = c * (t /= d)*t*t + b;
    }
    YTENewAction(easeOut)
    {
      value = c * ((t = t / d - 1)*t*t + 1) + b;
    }
    YTENewAction(easeInOut)
    {
      if ((t /= d / 2) < 1)
      {
        value = c / 2 * t*t*t + b;
      }
      else
      {
        value = c / 2 * ((t -= 2)*t*t + 2) + b;
      }
    }
  }

  namespace Elastic
  {
    YTENewAction(easeIn)
    {
      if (t == 0)
      {
        value = b;
      }
      else if ((t /= d) == 1)
      {
        value = b + c;
      }
      else
      {
        float p = d * .3f;
        float a = c;
        float s = p / 4;
        float postFix = static_cast<float>(a*pow(2, 10 * (t -= 1))); // this is a fix, again, with post-increment operators
        value = -(postFix * sinf((t*d - s)*(2 * glm::pi<float>()) / p)) + b;
      }
    }

    YTENewAction(easeOut)
    {
      if (t == 0)
      {
        value = b;
      }
      else if ((t /= d) == 1)
      {
        value = b + c;
      }
      else
      {
        float p = d * .3f;
        float a = c;
        float s = p / 4;
        value = static_cast<float>(a*pow(2, -10 * t) * sin((t*d - s)*(2 * glm::pi<float>()) / p) + c + b);
      }
    }

    YTENewAction(easeInOut)
    {
      if (t == 0)
      {
        value = b;
      }
      if ((t /= d / 2) == 2)
      {
        value = b + c;
      }
      else
      {
        float p = d * (.3f*1.5f);
        float a = c;
        float s = p / 4;

        if (t < 1)
        {
          float postFix = static_cast<float>(a*pow(2, 10 * (t -= 1)));
          value = -.5f*(postFix* sinf((t*d - s)*(2 * glm::pi<float>()) / p)) + b;
        }
        else
        {
          float postFix = static_cast<float>(a*pow(2, -10 * (t -= 1)));
          value = postFix * sinf((t*d - s)*(2 * glm::pi<float>()) / p)*.5f + c + b;
        }
      }

    }
  }

  namespace Expo
  {
    YTENewAction(easeIn)
    {
      value = (t == 0) ? b : static_cast<float>(c * pow(2, 10 * (t / d - 1)) + b);
    }

    YTENewAction(easeOut)
    {
      value = (t == d) ? b + c : static_cast<float>(c * (-pow(2, -10 * t / d) + 1) + b);
    }

    YTENewAction(easeInOut)
    {
      if (t == 0)
      {
        value = b;
      }
      else if (t == d)
      {
        value = b + c;
      }
      else if ((t /= d / 2) < 1)
      {
        value = c / 2 * static_cast<float>(pow(2, 10 * (t - 1)) + b);
      }
      else
      {
        value = c / 2 * static_cast<float>(-pow(2, -10 * --t) + 2) + b;
      }
    }
  }

  namespace Linear
  {
    YTENewAction(easeNone)
    {
      value = c * t / d + b;
    }
    YTENewAction(easeIn)
    {
      value = c * t / d + b;
    }
    YTENewAction(easeOut)
    {
      value = c * t / d + b;
    }
    YTENewAction(easeInOut)
    {
      value = c * t / d + b;
    }
  }

  namespace Quad
  {
    YTENewAction(easeIn)
    {
      value = c * (t*t) / (d*d) + b;
    }

    YTENewAction(easeOut)
    {
      value = c * (t*t) / (d*d) + b;
    }

    YTENewAction(easeInOut)
    {
      if ((t /= d / 2) < 1)
      {
        value = ((c / 2)*(t*t)) + b;
      }
      else
      {
        value = -c / 2 * (((t - 2)*(--t)) - 1) + b;
      }
    }
  }

  namespace Quart
  {
    YTENewAction(easeIn)
    {
      value = c * (t /= d)*t*t*t + b;
    }

    YTENewAction(easeOut)
    {
      value = -c * ((t = t / d - 1)*t*t*t - 1) + b;
    }

    YTENewAction(easeInOut)
    {
      if ((t /= d / 2) < 1)
      {
        value = c / 2 * t*t*t*t + b;
      }
      else
      {
        value = -c / 2 * ((t -= 2)*t*t*t - 2) + b;
      }
    }

  }

  namespace Quint
  {
    YTENewAction(easeIn)
    {
      value = c * (t /= d)*t*t*t*t + b;
    }
    YTENewAction(easeOut)
    {
      value = c * ((t = t / d - 1)*t*t*t*t + 1) + b;
    }

    YTENewAction(easeInOut)
    {
      if ((t /= d / 2) < 1)
      {
        value = c / 2 * t*t*t*t*t + b;
      }
      else
      {
        value = c / 2 * ((t -= 2)*t*t*t*t + 2) + b;
      }

    }
  }

  namespace Sine
  {
    YTENewAction(easeIn)
    {
      value = -c * cosf(t / d * (glm::pi<float>() / 2)) + c + b;
    }
    YTENewAction(easeOut)
    {
      value = c * sinf(t / d * (glm::pi<float>() / 2)) + b;
    }

    YTENewAction(easeInOut)
    {
      value = -c / 2 * (cosf(glm::pi<float>()*t / d) - 1) + b;
    }
  }

}

#endif
