/*
 * MIT License
 *
 * Copyright (C) 2022 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "quickstandardminimizebutton_p.h"
#include "framelessquickutils.h"
#include <QtGui/qguiapplication.h>
#include <QtGui/qstylehints.h>
#include <QtQuick/private/qquickimage_p.h>
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickanchors_p.h>

static inline void initResource()
{
    Q_INIT_RESOURCE(framelesshelperquick);
}

FRAMELESSHELPER_BEGIN_NAMESPACE

using namespace Global;

FRAMELESSHELPER_STRING_CONSTANT2(DarkUrl, "qrc:///org.wangwenx190.FramelessHelper/images/dark/chrome-minimize.svg")
FRAMELESSHELPER_STRING_CONSTANT2(LightUrl, "qrc:///org.wangwenx190.FramelessHelper/images/light/chrome-minimize.svg")

QuickStandardMinimizeButton::QuickStandardMinimizeButton(QQuickItem *parent) : QQuickButton(parent)
{
    initialize();
}

QuickStandardMinimizeButton::~QuickStandardMinimizeButton() = default;

void QuickStandardMinimizeButton::updateForeground()
{
    const bool dark = (FramelessQuickUtils::darkModeEnabled() || FramelessQuickUtils::titleBarColorized());
    const auto url = QUrl(dark ? kDarkUrl : kLightUrl);
    initResource();
    m_image->setSource(url);
}

void QuickStandardMinimizeButton::updateBackground()
{
    static constexpr const auto button = SystemButtonType::Minimize;
    const ButtonState state = (isPressed() ? ButtonState::Pressed : ButtonState::Hovered);
    const bool visible = (isHovered() || isPressed());
    m_backgroundItem->setColor(FramelessQuickUtils::getSystemButtonBackgroundColor(button, state));
    m_backgroundItem->setVisible(visible);
}

void QuickStandardMinimizeButton::updateToolTip()
{
    const bool visible = (isHovered() && !isPressed());
    const int delay = QGuiApplication::styleHints()->mousePressAndHoldInterval();
    m_tooltip->setVisible(visible);
    m_tooltip->setDelay(delay);
}

void QuickStandardMinimizeButton::initialize()
{
    setImplicitWidth(kDefaultSystemButtonSize.width());
    setImplicitHeight(kDefaultSystemButtonSize.height());

    m_contentItem.reset(new QQuickItem(this));
    m_contentItem->setImplicitWidth(kDefaultSystemButtonIconSize.width());
    m_contentItem->setImplicitHeight(kDefaultSystemButtonIconSize.height());
    m_image.reset(new QQuickImage(m_contentItem.data()));
    const auto imageAnchors = new QQuickAnchors(m_image.data(), m_image.data());
    imageAnchors->setCenterIn(m_contentItem.data());
    const FramelessQuickUtils * const utils = FramelessQuickUtils::instance();
    connect(utils, &FramelessQuickUtils::darkModeEnabledChanged, this, &QuickStandardMinimizeButton::updateForeground);
    connect(utils, &FramelessQuickUtils::titleBarColorizedChanged, this, &QuickStandardMinimizeButton::updateForeground);

    m_backgroundItem.reset(new QQuickRectangle(this));
    QQuickPen * const border = m_backgroundItem->border();
    border->setWidth(0.0);
    border->setColor(kDefaultTransparentColor);
    connect(this, &QuickStandardMinimizeButton::hoveredChanged, this, &QuickStandardMinimizeButton::updateBackground);
    connect(this, &QuickStandardMinimizeButton::pressedChanged, this, &QuickStandardMinimizeButton::updateBackground);

    m_tooltip = qobject_cast<QQuickToolTipAttached *>(qmlAttachedPropertiesObject<QQuickToolTip>(this));
    m_tooltip->setText(tr("Minimize"));
    connect(QGuiApplication::styleHints(), &QStyleHints::mousePressAndHoldIntervalChanged, this, [this](int interval){
        Q_UNUSED(interval);
        updateToolTip();
    });
    connect(this, &QuickStandardMinimizeButton::hoveredChanged, this, &QuickStandardMinimizeButton::updateToolTip);
    connect(this, &QuickStandardMinimizeButton::pressedChanged, this, &QuickStandardMinimizeButton::updateToolTip);

    updateBackground();
    updateForeground();
    updateToolTip();

    setContentItem(m_contentItem.data());
    setBackground(m_backgroundItem.data());
}

FRAMELESSHELPER_END_NAMESPACE
