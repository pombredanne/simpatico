#ifndef PARAM_TEST_COMPOSITE_H
#define PARAM_TEST_COMPOSITE_H

#include <test/CompositeTestRunner.h>

#include "LabelTest.h"
#include "BeginEndTest.h"
#include "ParamTest.h"
#include "ParamCompositeTest.h"
#include "ManagerTest.h"
//#include "TextCompositeTest.h"

TEST_COMPOSITE_BEGIN(ParamTestComposite)
TEST_COMPOSITE_ADD_UNIT(LabelTest);
TEST_COMPOSITE_ADD_UNIT(BeginEndTest);
TEST_COMPOSITE_ADD_UNIT(ParamTest);
TEST_COMPOSITE_ADD_UNIT(ParamCompositeTest);
TEST_COMPOSITE_ADD_UNIT(ManagerTest);
//TEST_COMPOSITE_ADD_UNIT(TextCompositeTest);
TEST_COMPOSITE_END

#endif
