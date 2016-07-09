#pragma once
/* 2016.06.23 build by simon(chenximeng@jiapartshf.com)
 * 用于记录duilib创建UI及其相关组件的名称,不同的UI界面进行
 * 分块进行宏定义,并以//! 30个'-'10个'*''注释内容'10个'*'30个'-'区
 * 开,宏定义规则【control类型_业务属性_功能】.注释内容:最好是当前
 * 页面具有统筹意义的可见文本,如没有尽量将注释写的易读易查
*/

//---------------------------------**********编辑合约文件***********------------------------------
#define XML_CONTRACT_FILE_NAME      _T("group_chat_edit_contract_page.xml")		//! xml文件名称

#define BTN_CONTRACT_PAGE_FILE		_T("gm_btn_contract_file")					//! 文件
#define BTN_CONTRACT_PAGE_DATA		_T("gm_btn_contract_data")					//! 资料
#define BTN_CONTRACT_PAGE_SHOOT		_T("gm_btn_contract_shoot")					//! 截图
#define BTN_CONTRACT_PAGE_CAR		_T("gm_btn_contract_car")					//! 车型
#define BTN_CONTRACT_PAGE_VIN		_T("gm_btn_contract_vin")					//! VIN码
#define BTN_CONTRACT_PAGE_SEND		_T("gm_btn_contract_send")					//! 发送合约

#define RADIO_CONTRACT_PAGE_IN		_T("OptionIn")								//! 收款方
#define RADIO_CONTRACT_PAGE_OUT		_T("OptionOut")								//! 付款方
#define RADIO_CONTRACT_PAGE_NONE	_T("OptionNone")							//! 不委托平台记账
#define RADIO_CONTRACT_PAGE_SURE	_T("OptionSure")							//! 担保交易
#define RADIO_CONTRACT_PAGE_AGREE	_T("OptionAgree")							//! 协议支付(周结)
#define RADOI_CONTRACT_PAGE_LOGISTICS   _T("OptionLogistics")					//! 物流代收

#define EDIT_CONTRACT_PAGE_NAME		_T("gm_edit_contract_name")					//! 合约名称
#define EDIT_CONTRACT_PAGE_MONEY	_T("gm_edit_money")							//! 合约金额
#define REDIT_CONTRACT_PAGE_INFO	_T("gm_richedit_contract_info")				//! 备注

#define PACKET_CONTRACT_PAGE_FILES  _T("gm_contractpackets")					//! 文件容器

//---------------------------------**********主对话框***********------------------------------
#define XML_MAIN_DIALOG_FILE_NAME	_T("main_dialog.xml")

#define RADIO_MAINDLG_PAGE_CONTRACT _T("opt_maidlg_contract")						//! 主对话框签约按钮

//---------------------------------**********签约详情主界面***********------------------------------
#define XML_CONTRACT_MAIN_PAGE_NAME _T("tab_contract_select.xml")

#define RADIO_CONTRACT_PAYEE		_T("option_payee")								//! 我是收款方
#define RADIO_CONTRACT_DRAWEE		_T("option_drawee")								//! 我是付款方

#define EDIT_SEARCH_KEY_WORD			_T("edit_key_word")							//! 关键字
#define DATE_SEARCH_START_TIME			_T("date_start_time")						//! 搜索开始时间
#define DATE_SEARCH_END_TIME			_T("date_end_time")							//! 搜索截止时间
#define COMB_SEARCH_PAY_TYPE			_T("combo_pay_type")						//! 支付方式
#define BTN_SEARCH_OK					_T("button_search")							//! 查询
#define LIST_CONTRACT_ITEMS				_T("contract_list")							//! 合同列表

#define LABEL_CONTRACT_ID				_T("label_contract_id")						//! 合约单号
#define LABEL_CONTRACT_NAME				_T("label_contract_name")					//! 合约名称
#define LABEL_CONTRACT_IDENTITY_TAG		_T("label_contract_identity_tag")			//! 收款方/付款方
#define LABEL_CONTRACT_IDENTITY			_T("label_contract_identity")				
#define BTN_CONTRACT_MSG				_T("btn_contract_msg")						//! 发送消息
#define BTN_CONTRACT_PHONR				_T("btn_contract_phone")					//! 打电话
#define LABEL_CONTRACT_MONEY			_T("label_contract_money")					//! 合约金额
#define LABEL_CONTRACT_PAY_TYPR			_T("label_contract_pay_type")				//! 支付方式
#define LABEL_CONTRACT_TIME				_T("label_contract_time")					//! 签约时间
#define LIST_CONTRACT_FILES				_T("list_contract_files")					//! 合约文件
#define ELEMENT_ITEM_NAME				_T("contract_element")
#define LABEL_CONTRACT_REMARK			_T("label_contract_remark")					//! 备注

#define BTN_LIST_PAGE_FIRST					_T("_btn_page_first")										//! 第一页
#define BTN_LIST_PAGE_PREV					_T("_btn_page_prev")										//! 上一页
#define BTN_LIST_PAGE_NEXT					_T("_btn_page_next")										//! 下一页
#define BTN_LIST_PAGE_END					_T("_btn_page_end")										//! 最后一页
//---------------------------------**********合约详情项***********------------------------------
#define XML_ITEM_CONTRACT_NAME			_T("item_contract_header.xml")
#define LABEL_ITEM_CONTRACT_WHO			_T("item_contract_who")						//! 合约所属
#define LABEL_ITEM_CONTRACT_NAME		_T("item_label_contract_name")				//! 合约名称
#define LABEL_ITEM_CONTRACT_MONEY		_T("item_label_contract_money")				//! 合约金额
#define LABEL_ITEM_CONTRACT_TIME		_T("item_label_contract_time")				//! 签约时间