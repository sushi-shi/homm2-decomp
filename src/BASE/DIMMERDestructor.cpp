#include <va.h>
#include <BASE/widget.h>
#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
template<class BaseWidget>
VA(0x004d3470, 0x1c)
heroWindow::DimmerWidget<BaseWidget>::~DimmerWidget() {}

template class heroWindow::DimmerWidget<widget>;
