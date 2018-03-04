#pragma once

#include <QColor>
#include <QColorDialog>
#include <QPushButton>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>

#include "YTE/Core/Utilities.hpp"

namespace YTEditor
{
  inline QColor ToQColor(glm::vec4 aColor)
  {
    QColor color;
    color.setRgbF(aColor.x, aColor.y, aColor.z, aColor.w);

    return color;
  }

  inline glm::vec4 FromQColor(QColor aColor)
  {
    double x, y, z, w;

    aColor.getRgbF(&x, &y, &z, &w);

    glm::vec4 color{ x, y, z, w };
    return color;
  }

  class ColorPicker : public QPushButton
  {
  public:
    explicit ColorPicker(const QColor &aColor,
                         bool aAlpha,
                         QWidget *aParent);

    glm::vec4 GetColor();
    void SetColor(const QColor &aColor);
    void SetColor(const glm::vec4 &aColor);
    void SetColor(const glm::vec3 &aColor);
    void clicked(bool aChecked);
    void paintEvent(QPaintEvent *aEvent);

  private:
    glm::vec4 mCurrentColor;
    bool mAlpha;
  };
}