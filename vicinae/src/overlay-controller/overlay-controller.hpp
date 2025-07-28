#pragma once
#include "common.hpp"
#include "ui/overlay/overlay.hpp"
#include <qobject.h>
#include <qtmetamacros.h>

class OverlayController : public QObject, NonCopyable {
  Q_OBJECT

  ApplicationContext *m_ctx = nullptr;
  QObjectUniquePtr<OverlayView> m_current = nullptr;

public:
  void setCurrent(OverlayView *view) {
    view->setContext(m_ctx);
    emit currentOverlayChanged(view);
    m_current.reset(view);
  }
  void dismissCurrent() {
    emit currentOverlayDismissed();
    m_current.reset();
  }

  OverlayController(ApplicationContext *ctx) : m_ctx(ctx) {}

signals:
  void currentOverlayChanged(OverlayView *view) const;
  void currentOverlayDismissed() const;
};
