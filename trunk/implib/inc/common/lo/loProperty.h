#ifndef __lo_PROPERTY_H__
#define __lo_PROPERTY_H__

// namespace locom
DEFINE_NAMESPACE(locom)

/** 属性值
*
* @author loach
*
* @date 2009-06-21
*/
template< typename PROPERTY_KEY>
struct loNovtableM IloPropertyItem
{
	/** 添加一项(如果存在,就进行修改) */
	virtual int Add(const PROPERTY_KEY& key,const char* value,size_t len) =0;

	/** 删除特定属性值 */
	virtual int Remove(const PROPERTY_KEY& key) =0;

	/** 清空所有属性值 */
	virtual void Clear(void) = 0;

	/** 获取相应的值 */
	virtual const char* Get(const PROPERTY_KEY& key) const =0;

	/** 获取相应的值 */
	virtual const char* Get(const PROPERTY_KEY& key , size_t& len) const = 0;

	/** 获取子属性值 */
	virtual IloPropertyItem<PROPERTY_KEY>* GetChild(void) const = 0;
	virtual bool IsChild(void) const = 0;
	virtual bool NewChild(void) = 0;	
};

END_NAMESPACE(locom)

#endif //