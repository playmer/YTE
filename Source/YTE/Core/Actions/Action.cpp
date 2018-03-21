#include "YTE/Core/Actions/Action.hpp"

namespace YTE
{
  Action::Action(float aDur)
    : mD(aDur)
    , mT(0)
    , mTime(mT)
  {
  }

  float Action::Increment(float dt)
  {
    mTime += dt;
    mT = mTime;
    if (mTime > mD)
    {
      mT = mD;
    }
    return mTime - mD;
  }

  bool Action::IsDone() const
  {
    return mTime >= mD;
  }

  void Action::Init()
  {
    // NOP
  }

  Action * Action::Clone() const
  {
    return new Action(*this);
  }

  void Action::operator()()
  {
    // NOP
  }

  Action::~Action()
  {
    // NOP
  }

  Action_Callback::Action_Callback(std::function<void(void)> aCallback)
    : Action_CRTP(0.1f, this)
    , mHasCalled(false)
    , mCallback(aCallback)
  {
  }

  Action_Callback::Action_Callback(const Action_Callback& aAction)
    : Action_CRTP(0.1f, this)
    , mHasCalled(aAction.mHasCalled)
    , mCallback(aAction.mCallback)
  {

  }

  void Action_Callback::operator()()
  {
    if (mHasCalled)
    {
      return;
    }
    mHasCalled = true;

    if (!mCallback)
    {
      return;
    }

    mCallback();
  }

  namespace Back
  {
    YTEDefineAction(easeIn)
    {
      float s = static_cast<float>(1.70158f);
      t /= d;
      float postFix = t;
      value = c * (postFix)*t*((s + 1)*t - s) + b;
    }
    YTEDefineAction(easeOut)
    {
      float s = static_cast<float>(1.70158f);
      value = c * ((t = t / d - 1)*t*((s + 1)*t + s) + 1) + b;
    }
    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = eIn(t, b, c, d);
    }
    YTEDefineAction(easeOut)
    {
      value = eOut(t, b, c, d);
    }

    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = -c * (sqrt(1 - (t /= d)*t) - 1) + b;
    }
    YTEDefineAction(easeOut)
    {
      value = c * sqrt(1 - (t = t / d - 1)*t) + b;
    }
    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = c * (t /= d)*t*t + b;
    }
    YTEDefineAction(easeOut)
    {
      value = c * ((t = t / d - 1)*t*t + 1) + b;
    }
    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
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

    YTEDefineAction(easeOut)
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

    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = (t == 0) ? b : static_cast<float>(c * pow(2, 10 * (t / d - 1)) + b);
    }

    YTEDefineAction(easeOut)
    {
      value = (t == d) ? b + c : static_cast<float>(c * (-pow(2, -10 * t / d) + 1) + b);
    }

    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeNone)
    {
      value = c * t / d + b;
    }
    YTEDefineAction(easeIn)
    {
      value = c * t / d + b;
    }
    YTEDefineAction(easeOut)
    {
      value = c * t / d + b;
    }
    YTEDefineAction(easeInOut)
    {
      value = c * t / d + b;
    }
  }

  namespace Quad
  {
    YTEDefineAction(easeIn)
    {
      value = c * (t*t) / (d*d) + b;
    }

    YTEDefineAction(easeOut)
    {
      value = c * (t*t) / (d*d) + b;
    }

    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = c * (t /= d)*t*t*t + b;
    }

    YTEDefineAction(easeOut)
    {
      value = -c * ((t = t / d - 1)*t*t*t - 1) + b;
    }

    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = c * (t /= d)*t*t*t*t + b;
    }
    YTEDefineAction(easeOut)
    {
      value = c * ((t = t / d - 1)*t*t*t*t + 1) + b;
    }

    YTEDefineAction(easeInOut)
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
    YTEDefineAction(easeIn)
    {
      value = -c * cosf(t / d * (glm::pi<float>() / 2)) + c + b;
    }
    YTEDefineAction(easeOut)
    {
      value = c * sinf(t / d * (glm::pi<float>() / 2)) + b;
    }

    YTEDefineAction(easeInOut)
    {
      value = -c / 2 * (cosf(glm::pi<float>()*t / d) - 1) + b;
    }
  }
}
