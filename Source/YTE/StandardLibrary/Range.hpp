#pragma once

namespace YTE
{
  template <typename TemplateType>
  class Range
  {
  public:
    Range(const TemplateType &aBegin, const TemplateType &aEnd)
      : mBegin(aBegin), mEnd(aEnd) {};

    bool IsRange() { return mBegin != mEnd; }

    const TemplateType begin() const { return mBegin; }
    const TemplateType end() const { return mEnd; }
    TemplateType begin() { return mBegin; }
    TemplateType end() { return mEnd; }

    typename TemplateType::difference_type size() const { return mEnd - mBegin; }
  protected:
    TemplateType mBegin;
    TemplateType mEnd;
  };
}