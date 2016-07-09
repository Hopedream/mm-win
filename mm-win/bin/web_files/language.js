/**
 * Created by zhiyaoc on 15/3/19.
 */

var language = $.cookie('language');

if (!language) {
	language = "zh-CN";

	$.cookie("language", language, {path: '/'});
}

var languageData = ({
	load: {cn: "发送中", en: "Loading..."},
	cart: {cn: "购物车预览", en: "Order Preview"},
	total: {cn: "总共", en: "Price"},
	goods: {cn: "个物品", en: "goods"},
	number: {cn: "数量", en: "Numbers"},
	White: {cn: "白色", en: "White"},
	White_Need_base: {cn: "白色 需要底座", en: "White Need base"},
	Black: {cn: "黑色", en: "Black"},
	Black_Need_base: {cn: "黑色 需要底座", en: "Black Need base"},
	White_Do_not_need_base: {cn: "白色 不需要底座", en: "White Do not need base"},
	Black_Do_not_need_base: {cn: "黑色 不需要底座", en: "Black Do not need base"},
	cart_list: {cn: "查看购物车", en: "View Cart"},
	settlement: {cn: "结 算", en: "Checkout"},
	shopping_over: {cn: "购物满 &#165;<em>199</em> 免邮费", en: "Free shipping over $<em>99</em>"},
	free_postage: {cn: "免邮费", en: "free postage"},
	shopping_over_num: {cn: "199", en: "99"}
});


var LangugeManager = {
	language: language.replace('-', '_') == 'zh_CN' ? 'cn' : 'en',

	t: function(text) {
		var re = new RegExp(" ","g");
		text = text.replace(re, '_');

		var languageStr = 'languageData.'+text+"."+this.language;

		return eval(languageStr);
	}
};