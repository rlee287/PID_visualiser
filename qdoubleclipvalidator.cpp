#include "qdoubleclipvalidator.h"

#include <iostream>
QDoubleClipValidator::QDoubleClipValidator(QObject *parent)
    : QDoubleValidator(parent) {}

QDoubleClipValidator::QDoubleClipValidator(double bottom, double top,
                                           int decimals, QObject *parent)
    : QDoubleValidator(bottom, top, decimals, parent) {}

void QDoubleClipValidator::fixup(QString &input) {
  std::cout << "i" << std::endl;
  bool isaNum;
  double val = input.toDouble(&isaNum);
  if (!isaNum) {
    return;
  }
  if (val < bottom()) {
    input.clear();
    input.setNum(bottom());
  }
  if (val > top()) {
    input.clear();
    input.setNum(top());
  }
}
