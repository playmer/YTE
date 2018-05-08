#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ColorPicker.hpp"

namespace YTEditor
{
    ColorPicker::ColorPicker(const QColor &aColor,
                             bool aAlpha,
                             QWidget *aParent = nullptr)
      : QPushButton{ aParent }
      , mAlpha{aAlpha}
    {
      connect(this, &QPushButton::clicked, this, &ColorPicker::clicked);
      mCurrentColor = FromQColor(aColor);
    }

    glm::vec4 ColorPicker::GetColor()
    {
      return mCurrentColor;
    }

    void ColorPicker::SetColor(const QColor &aColor)
    {
      mCurrentColor = FromQColor(aColor);
    }

    void ColorPicker::SetColor(const glm::vec4 &aColor)
    {
      mCurrentColor = aColor;
    }

    void ColorPicker::SetColor(const glm::vec3 &aColor)
    {
      mCurrentColor = glm::vec4{aColor, 1.0f};
    }

    void ColorPicker::clicked(bool aChecked)
    {
      auto options = QColorDialog::ColorDialogOptions();

      if (mAlpha)
      {
        options = QColorDialog::ShowAlphaChannel;
      }

      QColor color = QColorDialog::getColor(ToQColor(mCurrentColor), 
                                            this, 
                                            "Color Picker",
                                            QColorDialog::ShowAlphaChannel);

      if (color.isValid())
      {
        SetColor(color);
      }
    }

    void ColorPicker::paintEvent(QPaintEvent *aEvent)
    {
      QPushButton::paintEvent(aEvent);

      int colorPadding = 5;

      QRect rect = aEvent->rect();
      QPainter painter(this);
      painter.setBrush(QBrush(ToQColor(mCurrentColor)));
      painter.setPen("#CECECE");
      rect.adjust(colorPadding, colorPadding, -1 - colorPadding, -1 - colorPadding);
      painter.drawRect(rect);
    }
}