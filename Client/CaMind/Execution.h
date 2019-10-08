#pragma once

template<typename OBJ, typename Ret = void, typename ...ARGS>
Ret Execution(OBJ* obj, Ret(OBJ::*memFcn)(ARGS ...), ARGS ... args)
{
	return (obj->*memFcn)(args...);
}


template<typename Ret = void, typename ...ARGS>
Ret GlobalExecution(Ret(*fcn)(ARGS ...), ARGS ... args)
{
	return fcn(args...);
}