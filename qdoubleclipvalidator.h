#ifndef QDOUBLECLIPVALIDATOR_H
#define QDOUBLECLIPVALIDATOR_H

#include <QString>
#include <QValidator>

class QDoubleClipValidator : public QDoubleValidator {
public:
  explicit QDoubleClipValidator(QObject *parent = nullptr);
  QDoubleClipValidator(double bottom, double top, int decimals,
                       QObject *parent = nullptr);
  virtual void fixup(QString &input);
};

#endif // QDOUBLECLIPVALIDATOR_H
