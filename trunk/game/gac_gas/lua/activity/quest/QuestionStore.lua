cfg_load "essay_question/QuestionControl_Common"   --条件参数设置表

cfg_load "essay_question/EssayQuestion_Common"
cfg_load "essay_question/box_question"
cfg_load "essay_question/task_question"

g_QuestionStoreTbl = 
{
		["EssayQuestion_Common"] = EssayQuestion_Common,
		["box_question"] = box_question,
		["task_question"] = task_question
}