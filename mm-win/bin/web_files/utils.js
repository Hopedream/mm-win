var logLevels = {debug: 0, info: 1, warning: 2, error: 3};
var logLevel = "warning";
var logEvent = false;

var MAX_ROOM_NAME_LENGTH = 100;

/** ---------- prototype extension ------------ **/
if (typeof String.prototype.contains !== "function") {
    String.prototype.contains = function(needle){
        return this.indexOf(needle) !== -1;
    };
}
if (typeof String.prototype.startsWith !== "function") {
    String.prototype.startsWith = function(needle){
        return this.indexOf(needle) === 0;
    };
}
if (typeof Array.prototype.map !== "function") {
    Array.prototype.map = function(callback){
        var ret = [];
        for (var i=0; i<this.length; i++) {
            ret.push(callback.call(null, this[i]));
        }
        return ret;
    };
}
if (typeof Date.prototype.now !== "function") {
    Date.prototype.null = function(){
        return new Date().getTime();
    };
}

// Fix console functions for stupid IE.
if (!console) {
    console = {log: function(msg){}};
} else if (!console.log) {
    console.log = function(msg){};
}
if (!console.debug) {
    console.debug = function (msg) {
        console.log("--debug-- " + msg);
    };
}
if (!console.group) {
    console.group = function (msg) {
        console.log(" [>> GROUP START >>] " + msg);
    };
    console.groupCollapsed = function(msg){
        console.log(" [>> GROUP START >>] " + msg);
    };
    console.groupEnd = function(){
        console.log(" [<< GROUP END <<]");
    };
}
if (!console.warn) {
    console.warn = function(msg) {
        console.log(" [WARNING] " + msg);
    };
}
if (!console.error) {
    console.error = function(msg) {
        console.log(" [ERROR]" + msg);
    };
}
if (!console.trace) {
    console.trace = function(){
        console.log(" [trace not supported by browser]");
    };
}

var Utils = {
    Date: {
        DATE_NORMAL: "m d, Y",
        DATE_MD: "m d",
        DATE_TIME: "h:i P",
        DATE_FULL_TIME: "h:i:s",
        DATE_UTC: "Y-m-dTh:i:sZ",
        DATE_LOCAL: "Y-m-d h:i:s",
        DATE_FULL_DT: "m d, Y h:iP",

        DATETIME_TYPE: {
            OBJECT: "Date object",
            TIMESTAMP: "timestamp",
            UTC: "UTC time",
            LOCAL: 'local time'
        },

        userTimezoneOffset: undefined,

        /**
         * Given a Date object, return a date time string with millisecond precise.
         * @param {Date} [date] if omitted, the current Date.
         * @returns {string} e.g. "2014-07-31 12:57:12.376"
         */
        getMilliTimestamp: function (date){
            if (!date) {
                date = new Date();
            } else {
                date = Utils.Date.normalizeDateTime(date, Utils.Date.DATETIME_TYPE.OBJECT);
            }
            return Utils.Date.normalizeDateTime(date, Utils.Date.DATETIME_TYPE.LOCAL);
//                .replace("T", " ")
//                .replace("Z", "." + date.getMilliseconds().toString().slice(0, 3));
        },

        /**
         * Given a Date object or date time string or timestamp, return the value
         * in the specified format(type).
         * @param datetime can be a Date, a date time string or a timestamp (in millisecond)
         * @param targetType values in Utils.Date.DATETIME_TYPE.
         * @returns {*}
         */
        normalizeDateTime: function (datetime, targetType){

            var varType = typeof datetime;

            var isUTC = false, isObject = false, isTimestamp = false;
            if (varType == "object" && datetime instanceof Date) {
                isObject = true;
            } else if (!isNaN(datetime)) {
                isTimestamp = true;
                datetime = parseFloat(datetime);
            } else if (varType == "string" && /^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$/.test(datetime)) {
                isUTC = true;
            }

            switch (targetType) {
                case Utils.Date.DATETIME_TYPE.TIMESTAMP:
                    if (isTimestamp){
                        return datetime;
                    }
                    if (isObject){
                        return datetime.getTime();
                    }
                    if (isUTC){
                        return Date.parse(datetime);
                    }
                    break;
                case Utils.Date.DATETIME_TYPE.OBJECT:
                    if (isObject){
                        return datetime;
                    }
                    if (isUTC || isTimestamp){
                        return new Date(datetime);
                    }
                    break;
                case Utils.Date.DATETIME_TYPE.UTC:
                    if (isUTC){
                        return datetime;
                    }
                    if (isObject){
                        return Utils.Date.getUTCTimeFromObject(datetime);
                    }
                    if (isTimestamp){
                        return Utils.Date.getUTCTimeFromObject(new Date(datetime));
                    }
                    break;
                case Utils.Date.DATETIME_TYPE.LOCAL:
                    if (isUTC || isTimestamp){
                        return Utils.Date.getLocalTimeFromObject(new Date(datetime));
                    }
                    if (isObject){
                        return Utils.Date.getLocalTimeFromObject(datetime);
                    }
                    break;
                default:
                    break;
            }

            Utils.log("Unable to normalize datetime " + datetime + " to " + targetType + " format.", "error");
            throw new Error("Unable to normalize datetime " + datetime + " to " + targetType + " format.");
        },

        /**
         * Return UTC date time string by given Date object
         * @param {Date} date
         * @returns {string}
         */
        getUTCTimeFromObject: function (date){

            var m = date.getUTCMonth() + 1,
                d = date.getUTCDate(),
                h = date.getUTCHours(),
                i = date.getUTCMinutes(),
                s = date.getUTCSeconds();

            m = m > 9 ? m : "0" + m;
            d = d > 9 ? d : "0" + d;
            h = h > 9 ? h : "0" + h;
            i = i > 9 ? i : "0" + i;
            s = s > 9 ? s : "0" + s;

            return Utils.Date.DATE_UTC.replace("Y", date.getUTCFullYear())
                .replace("m", m)
                .replace("d", d)
                .replace("h", h)
                .replace("i", i)
                .replace("s", s);
        },

        /**
         * Return LOCAL date time string by given Date object
         * @param {Date} date
         * @returns {string}
         */
        getLocalTimeFromObject: function (date){

            var m = date.getMonth() + 1,
                d = date.getDate(),
                h = date.getHours(),
                i = date.getMinutes(),
                s = date.getSeconds();

            m = m > 9 ? m : "0" + m;
            d = d > 9 ? d : "0" + d;
            h = h > 9 ? h : "0" + h;
            i = i > 9 ? i : "0" + i;
            s = s > 9 ? s : "0" + s;

            return Utils.Date.DATE_LOCAL.replace("Y", date.getFullYear())
                .replace("m", m)
                .replace("d", d)
                .replace("h", h)
                .replace("i", i)
                .replace("s", s);
        },

        /**
         * Return if the given two time are on same day.
         *
         * @param time1 can be anything acceptable by Utils.Date.normalizeDateTime()
         * @param time2 can be anything acceptable by Utils.Date.normalizeDateTime()
         * @param {bool} diffBySeconds if false, the date (2014-07-12) will be compared. Otherwise, the actual
         * time difference will be checked to see if exceeds 24 hours.
         * @returns {boolean}
         */
        isOnSameDay: function (time1, time2, diffBySeconds){

            if (diffBySeconds) {
                time1 = Utils.Date.normalizeDateTime(time1, Utils.Date.DATETIME_TYPE.TIMESTAMP);
                time2 = Utils.Date.normalizeDateTime(time2, Utils.Date.DATETIME_TYPE.TIMESTAMP);
                return Math.abs(time1 - time2) < 24 * 3600 * 1000;
            } else {
                time1 = Utils.Date.normalizeDateTime(time1, Utils.Date.DATETIME_TYPE.OBJECT);
                time2 = Utils.Date.normalizeDateTime(time2, Utils.Date.DATETIME_TYPE.OBJECT);
                return time1.getDate() == time2.getDate() &&
                        time1.getMonth() == time2.getMonth() &&
                        time1.getFullYear() == time2.getFullYear();
            }
        },

        /**
         * returns
         *      1m .... 59m
         *      1h .... 23h
         *      1d ... 999d
         *      no value
         * @param time can be anything acceptable by Utils.Date.normalizeDateTime()
         * @returns {boolean}
         */
        getTimeDiff: function (time) {
	        if (typeof time == "undefined" || time === "") {
		        return "";
	        }

	        time = Utils.Date.normalizeDateTime(time, Utils.Date.DATETIME_TYPE.TIMESTAMP);

	        var dateNow = new Date();
	        var timeNow = dateNow.getTime();
	        timeNow = Utils.Date.normalizeDateTime(timeNow, Utils.Date.DATETIME_TYPE.TIMESTAMP);
	        var timeDiff = timeNow - time;

	        var day = 24 * 3600 * 1000;
	        var hour = 3600 * 1000;
	        var min = 60 * 1000;

	        if ((timeDiff / day) >= 1) {

		        return Math.floor(timeDiff / day) + "d";

	        } else if ((timeDiff / hour) >= 1) {

		        return Math.floor(timeDiff / hour) + "h";

	        } else if ((timeDiff / min) >= 1) {
		        return  Math.floor(timeDiff / min) + "m";
	        } else {
		        return "1m";
	        }
        },

        /**
         * Format timestamp of given time according to the given timestamp.
         * You can specify two format here. One is for when the time is on the same day with current time.
         * The other is for when the time is on different day with current time. If it is omitted, no difference
         * of the format then.
         * You can also specify how to check if it's a different day. See Utils.Date.isOnSameDay() for details.
         *
         * @param {number} timestamp
         * @param {string} todayFormat support m,d,Y,H,h,i,s,P. default DATE_NORMAL
         * @param {string} diffDayFormat used if the Date is different from today. default DATE_NORMAL
         * @param {boolean} diffBySeconds if true, any time that is older than 24 hours will be considered as a different day,
         * otherwise a different day means the Date (year-month-Date) is different.
         * @return string
         */
        formatTimestamp: function (timestamp, todayFormat, diffDayFormat, diffBySeconds){

            if (typeof timestamp !== "number") {
                timestamp = new Number(timestamp).valueOf();
            }

            if (!todayFormat){
                todayFormat = Utils.Date.DATE_MD;
            }
            if (!diffDayFormat){
                diffDayFormat = Utils.Date.DATE_MD;
            }
            if (todayFormat == Utils.Date.DATE_UTC) {
                return Utils.Date.getUTCTimeFromObject(new Date(timestamp));
            }

            var date = new Date(timestamp), now = new Date();
            var isToday = Utils.Date.isOnSameDay(timestamp, now.getTime(), diffBySeconds);

            // if UserTimeZone is set, we"ll use custom timezone instead of local timezone.
            if (typeof Utils.Date.userTimezoneOffset == "number") {
                var userTzOffset = Utils.Date.userTimezoneOffset * 3600000;
                var utcTimestamp = timestamp + now.getTimezoneOffset() * 60000;
                date.setTime(utcTimestamp + userTzOffset);
            }

            var Y = date.getFullYear();
            var m = date.getMonth();
            var d = date.getDate();
            var H = date.getHours(), i = date.getMinutes(), s = date.getSeconds();
            var p = H < 12 ? "AM" : "PM";
            var h = H > 12 ? H - 12 : H;
            var monthAbbreviations = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];

            m = monthAbbreviations[m];
            d = d > 9 ? d : "0" + d;
//            H = H > 9 ? H : "0" + H;
//            h = h > 9 ? h : "0" + h;
	        H = H == 0 ? 12 : H;
	        h = h == 0 ? 12 : h;
            i = i > 9 ? i : "0" + i;
            s = s > 9 ? s : "0" + s;

            return (isToday ? todayFormat : diffDayFormat).replace("m", m)
                .replace("d", d)
                .replace("Y", Y)
                .replace("h", h).replace("H", H)
                .replace("i", i)
                .replace("s", s)
                .replace("P", p);
        }
    },

    DOM: {
        PARSE_ALL: 1,
        PARSE_ROW: 2,
        PARSE_COLUMN: 3,
        PARSE_SCALAR: 4,

        /**
         * Find the first child element of the node name.
         * If nodeName is not specified, simply return the first child element.
         * This does not limited to direct child.
         *
         * @param {Element} dom
         * @param {string} [nodeName]
         * @returns {Element|undefined}
         */
        firstChild: function (dom, nodeName){
            if (nodeName) {
                var nodes = dom.getElementsByTagName(nodeName);
                return nodes.length ? nodes[0] : undefined;
            } else {
                var children = $(dom).children();
                if (children.length) {
                    return children.eq(0)[0];
                } else {
                    return undefined;
                }
            }
        },

        /**
         * Select the child node according to a path string.
         * The divider is "/", and each part is considered as a nodeName.
         *
         * @param {Element} dom
         * @param {string} path e.g.  "history/item/author"
         * @returns {Element|undefined}
         */
        selectPath: function (dom, path){
            var nodeNames = path.split("/");
            var ret = dom;

            for(var i=0; i<nodeNames.length; i++){
                ret = Utils.DOM.firstChild(ret, nodeNames[i]);
                if (!ret){
                    break;
                }
            }
            return ret;
        },

        /**
         * Check if the DOM element contains an "X" element and of the given namespace.
         *
         * @param {Element} dom
         * @param {string} namespace
         * @returns {boolean}
         */
        isOfNameSpace: function (dom, namespace){
            if (dom.nodeName.toLowerCase() == "x") {
                return dom.getAttribute("xmlns") == namespace;
            }
            return Utils.DOM.getChildrenAttributes(dom, "x", ["xmlns"], Utils.DOM.PARSE_COLUMN).indexOf(namespace) !== -1;
        },

        /**
         * Retrieve the "X" node with given namespace.
         * @param dom
         * @param namespace
         * @returns {Element|null}
         */
        getXChildOfNS: function (dom, namespace){
            return Utils.DOM.getChildOfNS(dom, "x", namespace);
        },

        /**
         * Retrieve a node with given namespace and nodeName.
         *
         * @param {Element} dom
         * @param {string} childTagName
         * @param {string} namespace
         * @returns {Element|null}
         */
        getChildOfNS: function (dom, childTagName, namespace){

            var eList = dom.getElementsByTagName(childTagName), elem = null;

            $.each(eList, function(index, item){
                if (item.getAttribute("xmlns") == namespace) {
                    elem = item;
                    return false;
                }
                return true;
            });
            return elem;
        },

        /**
         * Return an object that has the specified attributes of the element.
         *
         * @param {Element} dom
         * @param {Array} attrNames
         * @returns {Object}
         */
        getAttributesOfNode: function (dom, attrNames){
            var data = {};
            if (attrNames instanceof Array) {
                $.each(attrNames, function(index, key){
                    data[key] = dom.getAttribute(key);
                });
            }
            return data;
        },

        /**
         * Retrieve specified attributes from children elements from the node.
         * According to the value of parseMode, the result will be different.
         *
         * PARSE_SCALAR: return is a string, the value of the first attribute listed in attrNames of the first
         *   <childTagName> child of the dom.
         * PARSE_ROW: return is an object, the attribute-values of the first <childTagName> child of the dom.
         *   (attributes listed in attrName. For non-existed attribute, value is undefined)
         * PARSE_COLUMN: return is an array, the values of the first attribute listed in attrNames of all
         *   <childTagName> children of the dom.
         * PARSE_ALL: return is an array, each is an object, the attribute-values of the first <childTagName>
         *   child of the dom. (attributes listed in attrName. For non-existed attribute, value is undefined)
         *
         * @param {Element} dom
         * @param {string} childTagName
         * @param {Array|false} attrNames
         * @param parseMode [optional] can be PARSE_ALL (default), PARSE_ROW, PARSE_COLUMN, PARSE_SCALAR
         * @returns {Array|string}
         */
        getChildrenAttributes: function (dom, childTagName, attrNames, parseMode){

            if ([Utils.DOM.PARSE_ALL, Utils.DOM.PARSE_ROW, Utils.DOM.PARSE_COLUMN, Utils.DOM.PARSE_SCALAR].indexOf(parseMode) === -1) {
                parseMode = Utils.DOM.PARSE_ALL;
            }

            var data = parseMode == Utils.DOM.PARSE_SCALAR ? undefined : (parseMode == Utils.DOM.PARSE_ROW ? {} : []);
            var items = dom.getElementsByTagName(childTagName), item;
            if (!items.length) {
                return data;
            }

            if (attrNames === false) { // parse for node text
                //TODO remove this part. use Utils.DOM.Dom.getChildrenValues() instead.
                if (parseMode == Utils.DOM.PARSE_SCALAR) {
                    data = Utils.DOM.getStropheText(items[0]);
                } else {
                    $.each(items, function(index, item){
                        data.push(Utils.DOM.getStropheText(item));
                    });
                }
            }else if (attrNames === 'ALL') {
	            for (var i=0; i<items.length; i++){
		            item = items[i];
		            var row = {};
		            $.each(item.attributes, function(index, attr){
			            try{
				            row[attr.name] = JSON.parse(Utils.String.xmlUnescape(attr.value));
			            }catch (e){
				            row[attr.name] = Utils.String.xmlUnescape(attr.value);
			            }
		            });
		            data.push(row);
	            }
	        }else if (attrNames.length) { // parse for node attributes
                switch (parseMode) {
                    case Utils.DOM.PARSE_ALL:
                        for (var i=0; i<items.length; i++){
                            item = items[i];
                            var row = {};
                            $.each(attrNames, function(index, key){
                                row[key] = item.getAttribute(key);
                            });
                            data.push(row);
                        }
                        break;

                    case Utils.DOM.PARSE_ROW:
                        item = items[0];
                        $.each(attrNames, function(index, key){
                            data[key] = item.getAttribute(key);
                        });
                        break;

                    case Utils.DOM.PARSE_COLUMN:
                        var attrName = attrNames[0];
                        $.each(items, function(index, item){
                            data.push(item.getAttribute(attrName));
                        });
                        break;

                    case Utils.DOM.PARSE_SCALAR:
                        data = items[0].getAttribute(attrNames[0]);
                        break;

                    default:
                        break;
                }
            } else {
                console.warn("Empty "+attrNames+" argument for getChildrenAttributes().");
            }
            return data;
        },

        /**
         * Retrieve value of children elements from the node.
         * According to the value of parseMode, the result will be different.
         *
         * PARSE_SCALAR: return is a string, the value of the first child of the dom, which nodeName is the first
         *   one listed in childrenTagNames
         * PARSE_ROW: return is an object, the key is the tag name listed in childrenTagNames, the value is the
         *   value of corresponding node of the first one.
         *   (attributes listed in attrName. For non-existed attribute, value is undefined)
         * PARSE_COLUMN: return is an array, the values of the first attribute listed in attrNames of all
         *   <childTagName> children of the dom.
         * PARSE_ALL: return is an array, each is an object, the attribute-values of the first <childTagName>
         *   child of the dom. (attributes listed in attrName. For non-existed attribute, value is undefined)
         *
         * @param dom
         * @param childrenTagNames
         * @param parseMode
         * @returns {undefined}
         */
        getChildrenValues: function (dom, childrenTagNames, parseMode){

            var multiTags = Utils.isArray(childrenTagNames), singleTag = typeof childrenTagNames == "string";

            if (!multiTags && !singleTag) {
                throw new TypeError("Utils.DOM.getChildrenValues(): Expecting parameter 2 to be a String or array, " + typeof(childrenTagNames) + " given.");
            }
            if (multiTags && childrenTagNames.length === 0) {
                throw new Error("Utils.DOM.getChildrenValues(): parameter 2 is an empty array.");
            }

            var firstTagNodes = null,
                data = undefined;

            if (singleTag) {
                firstTagNodes = dom.getElementsByTagName(childrenTagNames);
            } else {
                firstTagNodes = dom.getElementsByTagName(childrenTagNames[0]);
            }

            switch (parseMode) {
                case Utils.DOM.PARSE_SCALAR:
                    data = Utils.DOM.getValueOfNode(firstTagNodes);
                    break;

                case Utils.DOM.PARSE_ROW:
                    data = {};
                    if (multiTags) {
                        childrenTagNames.map(function(tagName){
                            data[tagName] = Utils.DOM.getValueOfNode(dom.getElementsByTagName(tagName));
                        });
                    } else {
                        data[childrenTagNames] = Utils.DOM.getValueOfNode(firstTagNodes);
                    }
                    break;

                case Utils.DOM.PARSE_COLUMN:
                    data = Utils.DOM.getValuesOfNode(firstTagNodes);
                    break;

                case Utils.DOM.PARSE_ALL:
                    if (multiTags) {
                        data = {};
                        childrenTagNames.map(function(tagName){
                            data[tagName] = Utils.DOM.getValuesOfNode(dom.getElementsByTagName(tagName));
                        });
                    } else {
                        data = Utils.DOM.getValuesOfNode(firstTagNodes);
                    }
                    break;

                default:
                    throw new Error("Utils.DOM.getChildrenValues(): unknown parse mode: " + parseMode);
                    break;
            }

            return data;
        },

        /**
         *
         * @param {Element} node
         * @param {number} [level]
         */
        nodeToString: function (node, level){

            if (typeof level != "number") level = 0;
            if (node === null) return "null";
            if (typeof node != "object" || typeof node.nodeType == "undefined") {
                if (typeof node == "undefined") return "undefined";
                return node.toString();
            }

            switch (node.nodeType) {
                case 3:
                    return node.nodeValue;
                case 2:
                    return node.name + "=\"" + node.value + "\"";
                case 1:

                    var padding = "";
                    if (level) {
                        for (var i=level; i!=0; i--) padding += "    ";
                    }
                    var content = "\n" + padding + "<" + node.tagName;

                    var attrList = node.attributes;
                    if (attrList.length) {
                        content += " " + Utils.listToArray(attrList).map(function(attr){
                            return Utils.DOM.nodeToString(attr, level+1);
                        }).join(" ");
                    }

                    if (!node.hasChildNodes()) {
                        content += "/>";
                    } else {
                        content += ">";
                        content += Utils.listToArray(node.childNodes).map(function(childNode){
                            return Utils.DOM.nodeToString(childNode, level+1);
                        }).join("");

                        var isTextChild = content.charAt(content.length - 1) != ">";
                        if (!isTextChild) {
                            content += "\n" + padding;
                        }
                        content += "</" + node.tagName + ">";
                    }
                    return content;
                default:
                    return "";
                    break;
            }
        },

        /**
         *
         * @param elem
         * @param returnNullOnFalse
         * @returns {*}
         */
        ensureElement: function (elem, returnNullOnFalse){
            if (typeof elem == "object") {
                if (elem instanceof Element) return elem;
                if (elem instanceof Strophe.Builder) return elem.tree();
            }
            return returnNullOnFalse ? null : document.createElement('foo');
        },

        /**
         *
         * @param {Element} response
         * @param {Element|Strophe.Builder} request
         * @returns {object} {type: String, code: String, msg: String}
         */
        parseError: function (response, request){

            response = Utils.DOM.ensureElement(response, true);
            request = Utils.DOM.ensureElement(request);

            if (!response) {
                return new XMPPErrorResponse("Unknown error. Possibly request timeout.", "unknown", "0", request.getAttribute("id"));
            }

            var errorNode = Utils.DOM.firstChild(response, "error");
            if (errorNode) {

                var errorMsg = "";
                var firstNode = Utils.DOM.firstChild(errorNode);
                if (firstNode) {
                    errorMsg = firstNode.tagName ? firstNode.tagName.toLowerCase() : firstNode.nodeName.toLowerCase();
                    errorMsg = Utils.String.ucwords(errorMsg.replace("-", " "));
                }
                var extraText = Utils.DOM.getChildrenValues(errorNode, "text", Utils.DOM.PARSE_SCALAR);
                if (extraText) {
                    errorMsg = "[" + errorMsg + "] " + extraText;
                }

                var attrs = Utils.DOM.getAttributesOfNode(errorNode, ["type", "code"]);
                return new XMPPErrorResponse(errorMsg, attrs.type, attrs.code, response.getAttribute("id") || request.getAttribute("id"));
            } else {
                return new XMPPErrorResponse("(No error node found in reponse)");
            }
        },

        /**
         *
         * @param item
         * @returns {string}
         */
        getStropheText: function (item){
            //console.Utils.log(Utils.DOM.nodeToString(item));
            return Utils.String.xmlUnescape(Strophe.getText(item)); //TODO this is not necessary actually. waiting for server to remove the unnecessary escaping
        },

        /**
         *
         * @param nodes
         * @returns {string}
         */
        getValueOfNode: function (nodes){
            return nodes.length ? Utils.DOM.getStropheText(nodes[0]) : undefined;
        },

        /**
         *
         * @param nodes
         * @returns {Array}
         */
        getValuesOfNode: function (nodes){
            var data = [];
            if (nodes.length) {
                for (var i= 0, l=nodes.length; i<l; i++){
                    data.push(Utils.DOM.getStropheText(nodes[i]));
                }
            }
            return data;
        }
    },

    String: {
	    getUidFromEmail: function(jid){
			return jid.split('@')[0];
	    },
	    getDomainFromEmail: function(jid){
		    return jid.split('@')[1];
	    },
        emailPattern: /^[a-zA-Z0-9!#$%&\'*+\\/=?^_`{|}~-]+(?:\.[a-zA-Z0-9!#$%&\'*+\\/=?^_`{|}~-]+)*@(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]*[a-zA-Z0-9])?\.)+[a-zA-Z0-9](?:[a-zA-Z0-9-]*[a-zA-Z0-9])?$/,
        /**
         *
         * @param string
         * @returns {boolean}
         */
        isImageData: function (string){
            return typeof string == "string" && string.indexOf("data:image/") === 0;
        },

        isEmail: function(string){
            return Utils.String.emailPattern.test(string);
        },

        /**
         *
         * @param str
         * @returns {*}
         */
        ucfirst: function (str){
            if (str.length) {
                return str.charAt(0).toUpperCase() + str.slice(1);
            }
            return str;
        },

        /**
         *
         * @param str
         * @returns {*}
         */
        ucwords: function (str){
            if (str.length) {
                return Utils.String.ucfirst(str).replace(/( +\w)/g, function(v){
                    return v.toUpperCase();
                });
            }
            return str;
        },

        /**
         *
         * @param str
         * @returns {*}
         */
        rtrim: function (str){
            if (str.length) {
                return str.replace(/(\s+)$/, "");
            }
            return str;
        },

        /**
         *
         * @param str
         * @returns {XML|string|void}
         */
        htmlEncodeSpace: function (str){
            return str.replace(/  /g, function(v){
                return "&nbsp;&nbsp;";
            });
        },

        /**
         *
         * @returns {string}
         * @param str
         */
        xmlUnescape: function (str) {
            if (!str || str.length == 0){
                return "";
            }

            return str.replace(/&amp;/g, "&")
                .replace(/&lt;/g, "<")
                .replace(/&gt;/g, ">")
                .replace(/&nbsp;/g, " ")
                .replace(/"/g, "\"")
                .replace(/&quot;/g, "\"")
                .replace(/&apos;/g, "'");
        },

        /**
         *
         * @param str
         * @returns {*}
         */
        xmlescape: function (str) {
            if (!str || str.length == 0){
                return "";
            }

            return str.replace(/&/g, "&amp;")
	            .replace(/<br\/>/g, "\r\n")
                .replace(/</g, "&lt;")
                .replace(/>/g, "&gt;")
                .replace(/ /g, "&nbsp;")
                .replace(/"/g, "&quot;")
                .replace(/'/g, "&apos;");
        },


        /**
         * get file html link by source
         *  return <a href="http://test.dd/test.png" target="_blank">test.png</a>
         * @returns string
         *
         * @param source
         */
        get_file_link: function (source){

            var nameOnly = arguments[1] ? arguments[1] : false;
            //source = decodeURIComponent(source);
            var urlPrefix = bui.getParam("filePicker").fileUrlPrefix

            var url_re = urlPrefix.replace(/\//g,"\\/"); // prepare the fixed URL (file picker) to be used in regex below

            var re= new RegExp("("+ url_re +"[^\?^\+]*)(\\\?filename=|\\\+)(.+)$", "i");
            // Test example1: source = https://www.filepicker.io/api/file/OqDu2OdQYeG5zkup8XwE+IMG_3059_XXX.png
            // Test example2: source = https://www.filepicker.io/api/file/OqDu2OdQYeG5zkup8XwE?filename=IMG_3059_XXX.png
            // match[1] should be: https://www.filepicker.io/api/file/OqDu2OdQYeG5zkup8XwE
            // match[3] should be: IMG_3059_XXX.png

            var match = source.match(re);

            if (match) {  // we have a file picker URL
                //var url  = match[1]; // full URL to display the image

                var url = match[1];
                var name = decodeURIComponent(match[3]); // file name
                if (nameOnly) {
                    return "Uploaded file: "+name;
                }
                var thumburl = url+"/convert?h=150&fit=max";  // file picker specific
                var previewUrl = url;  // file picker specific
                var extension_arr =name.split(".");
                var ext = extension_arr[extension_arr.length-1];
                var toggleHtml = '';
                var display = 'inline-block';

	            var playBody = '<audio controls="controls" src="'+url+'"></audio>';

	            if ($.browser.msie) {
		            toggleHtml = '"data-toggle="tooltip" title="Please Read It By Chrome or Firefox."';
		            display = "none";

	            }
	            if ($.browser.mozilla) {
		            playBody = '<embed width="300px" height="100%" name="plugin" src="' + url + '" autostart="false" type="audio/x-m4a">';
	            }

                if(/^jpg|png|gif|jpeg|bmp$/i.test(ext)){
                    return '<a class="file-name" href="' + url +
                        '" target="_blank" style="text-decoration: underline; overflow: hidden; text-overflow: ellipsis;' +
                        'white-space: nowrap; word-break: normal; word-wrap: break-word; display: inline-block; max-width: 90%;">'
                        + name +
                        '</a><div style="margin-top:15px"><a href="'+previewUrl+'" class="imgprew"> <img src="'+thumburl+'"></a>' +
                        '<div class="filepick-buttons">' +
                        '<a href="' + url + '"" target="_blank"><span class="icon-zoom-in"></span> Open original</a>' +
                        '<a href="' + url + '?dl=true" target="_blank"><span class="icon-download"></span> Download</a>' +
                        '</div>' +
                        '</div>';
                }else if(/^m4a|mp3$/i.test(ext)){
                    return '<span class="voice"></span><span style="width:170px; overflow:hidden; text-overflow:ellipsis; white-space:nowrap;' +
                        'display: block; margin-left: 25px; margin-top: 4px; height: 16px; padding: 1px; line-height: 16px; position: absolute; "'+toggleHtml+'>'
                        + extension_arr[0] +
                        '</span>' +
                        '<div id="voice-content" style="vertical-align: middle; left: 172px; margin-top: -1px; position: relative; display: '+display+'; height: 32px; width: 32px; ">'
                        + playBody +
                        '</div><br>';
                }else{
                    return '<a href="' + url + '" target="_blank" style="text-decoration:underline;">' + name + '</a>';
                }

            } else {
                if(nameOnly){
                    return source;
                }
                var matched = false;
                var regExp = /((http(s?):\/\/)(([A-Z0-9][A-Z0-9_-]*)(\.[A-Z0-9][A-Z0-9_-]*)+)(:[\d]+)?(\/[^\s]*)*)/gi;
                source = source.replace(regExp, function(all, url){
                    matched = true;
                    url = decodeURI(url);
                    return '<a style="text-decoration:underline;" href="' + url + '" target="_blank">' + url + '</a>';
                });
                if(!matched){
                    var regExp2 =/([^\/|^](www\.([A-Z0-9][A-Z0-9_-]*)(\.[A-Z0-9][A-Z0-9_-]*)+)(:[\d]+)?(\/[^\s]*)*)/gi;
                    source = source.replace(regExp2, function(all, url){
                        matched = true;
                        url = decodeURI(url);
                        return '<a style="text-decoration:underline;" href="http://' + url + '" target="_blank">' + url + '</a>';
                    });
                }
                if(!matched){
                    var regExp3 =/((^www\.([A-Z0-9][A-Z0-9_-]*)(\.[A-Z0-9][A-Z0-9_-]*)+)(:[\d]+)?(\/[^\s]*)*)/gi;
                    source = source.replace(regExp3, function(all, url){
                        url = decodeURI(url);
                        return '<a style="text-decoration:underline;" href="http://' + url + '" target="_blank">' + url + '</a>';

                    });
                }
                return source;

            }

        },
        strcut: function(str,start,len,suffix){
            var result='';
            var j=0;
            var pa=new RegExp(/\s/);
            if( typeof str == "undefined" || str.length==0){
                return result;
            }
            if(typeof start == "undefined"){
                start=0;
            }else{
                start=parseInt(start);
            }
            len=parseInt(len);
            if( typeof suffix == "undefined"){
                suffix='...';
            }
            if(str.charAt(41) == ''){
                return str;
            }
            //cal true length
            for(i=41;i>=0;i--){
                if(pa.test(str.charAt(i))){
                    j++;
                    break;
                }else{
                    j++;
                }
            }
            len=len-j;
            if(len<str.length){
                return str.substr(start,len)+suffix;
            }else{
                return str;
            }
        }
    },

    Ajax: {},

    /**
     *
     * @param v
     * @param depth
     * @returns {*}
     */
    dump: function (v, depth){
        if (typeof depth == "undefined"){
            depth = 1;
        } else {
            depth++;
        }

        var varType = typeof v;

        switch (varType) {
            case "string":
                return JSON.stringify(v);
            case "number":
                return v;
            case "boolean":
                return v.toString();
            case "undefined":
                return "undefined";
            case "function":
                return v.toString().replace(/\{(.*)\}$/, "{...}");
            case "object":
                return Utils.dumpObject(v, depth);
            default:
                return varType;
        }
    },

    /**
     *
     * @param obj
     * @returns {*}
     */
    dumpObject: function (obj, depth){
        var str = "";
        if (obj === null){
            return "NULL";
        }
        if (obj instanceof Array) {
            str = "Array(" + obj.length + ") [";
            if (depth > 3) {
                str += "...]";
            } else {
                for (var i= 0, l=obj.length; i<l; i++) {
                    str += Utils.dump(obj[i], depth) + ", ";
                }
                if (str.slice(-2) == ", "){
                    str = str.slice(0, -2);
                }
                str += "]";
            }
            return str;
        }
        if (obj instanceof Date) {
            return "Date (" + Date.toString() + ")";
        }
        if (obj instanceof Element) {
            switch (obj.nodeType) {
                case 3:
                    return "TextNode (length=" + obj.nodeValue.length +")";
                case 1:
                    return "ElementNode <" + obj.nodeName + ">";
                default:
                    return "Element (type=" + obj.nodeType + ")";
            }
        }
        if (obj.toString != Object.prototype.toString) { // this object has custom toString()
            return obj.toString();
        }
        var className = obj.constructor.name;
        if (className) {
            if (className == "Object") {
                if (depth > 3) {
                    str = "object {...}";
                } else {
                    str = "{";
                    for (var attr in obj) {
                        if (obj.hasOwnProperty(attr)) {
                            str += JSON.stringify(attr) + ": " + Utils.dump(obj[attr], depth) + ", ";
                        }
                    }
                    if (str.slice(-2) == ", "){
                        str = str.slice(0, -2);
                    }
                    str += "}";
                }
                return str;
            } else {
                return "object " + className;
            }
        }

        return obj.toString().replace(/^\[object (.+)\]$/, "$1");
    },

    /**
     *
     * @param v
     * @returns {boolean}
     */
    isArray: function (v) {
        return typeof v == "object" && v instanceof Array;
    },

    isObject: function(obj){
        return typeof obj == "object" && obj !== null;
    },

    /**
     *
     * @param arr
     * @param n
     * @returns {*}
     */
    deleteArr: function (arr , n) {
        if (n<0){
            return arr;
        } else {
            return arr.slice(0,n).concat(arr.slice(n+1,arr.length));
        }
    },

    /**
     *
     * @param item
     * @param arr
     * @returns {boolean}
     */
    in_array: function (item , arr){
        for (i=0;i<arr.length;i++){
            if(arr[i] == item)
                return true;
        }
        return false;
    },

    /**
     *
     * @param list
     * @returns {Array}
     */
    listToArray: function (list){
        return Array.prototype.slice.apply(list);
    },

    /**
     *
     * @param a
     * @param b
     * @returns {number}
     */
    strSort: function (a , b){
        var ret = 0,
	        length = Math.max(a.length, b.length);
        for (var i = 0, len = length; i<len; i++){
            if (a[i] != b[i]){
                if (!a[i]){
                    ret = -1;
                } else if (!b[i]){
                    ret = 1;
                } else {
                    if (a[i].toLowerCase() == b[i].toLowerCase()){
                        ret = a[i] < b[i] ? -1 : 1;
                    } else {
                        ret = a[i].toLowerCase() > b[i].toLowerCase() ? 1 : -1;
                    }
                }
                break;
            }
        }
        return ret;
    },

    /**
     *
     * @param content
     * @param [level]
     */
    log: function (content, level){
        consoleLog(content, level);
        if (level == "error") {
            // always push "error" Utils.log to server
            remoteLogger.logError(content);
        }
    },

    getUUID: (function(){
        var dec2hex = [];
        for (var i = 0; i <= 15; i++) {
            dec2hex[i] = i.toString(16);
        }

        return function() {
            var uuid = '';
            for (var i = 1; i <= 36; i++) {
                if (i === 9 || i === 14 || i === 19 || i === 24) {
                    uuid += '-';
                } else if (i === 15) {
                    uuid += 4;
                } else if (i === 20) {
                    uuid += dec2hex[(Math.random() * 4 | 0 + 8)];
                } else {
                    uuid += dec2hex[(Math.random() * 15 | 0)];
                }
            }
            return uuid;
        };
    })(),

    /**
     *
     * @param {function} func
     * @param {object} [owner]
     * @param [args]
     * @returns {*}
     */
    invokeFunction: function (func, owner, args){
        if (typeof func != "function"){
            return;
        }

        if (!owner){
            owner = null;
        }

        if (!(args instanceof Array)) { // auto wrap args into an array
            if (typeof args == "undefined") { // no args.
                args = [];
            } else { // only one args
                args = [args];
            }
        }

        try {
            return func.apply(owner, args);
        } catch (e) {
            Utils.dumpError(e);
            throw e;
        }
    },

    /**
     * This function will try to read attribute of given object.
     * It will try its getter method first, its attribute. If both failed,
     * the fallback will be returned (if not a function) or called, given
     * the first name as the attribute name, and the object as context.
     * If the trials failed and no fallback is provided, an warning will be
     * outputted in console.
     * Notice: you need to avoid dead loop yourself.
     *
     * @param {object} obj
     * @param {string} name
     * @param {function|mixed} [fallback]
     * @return mixed
     */
    readObjAttr: function(obj, name, fallback){

        var getter = "get" + Utils.String.ucfirst(name);
        if (typeof obj[getter] == "function") {
            return obj[getter].call(obj);
        } else if (obj.hasOwnProperty(name)) {
            return obj[name];
        } else if (arguments.length == 3){
            if (typeof fallback == "function") {
                return fallback.call(obj, name);
            } else {
                return fallback;
            }
        } else {
            console.warn("No fallback provided on reading object attribute failure.");
        }
    },

    /**
     * This function will try to write attribute of given object.
     * It will try its setter method first, its attribute. If both failed,
     * the fallbackFunc will be called, given the attribute name as the first
     * argument, the value to be set as the second argument, and the object
     * as context.
     * If the trials failed and no fallback is provided, an warning will be
     * outputted in console.
     * Notice: you need to avoid dead loop yourself.
     *
     * @param {object} obj
     * @param {string} name
     * @param {mixed} value
     * @param {function} [fallbackFunc]
     */
    writeObjAttr: function(obj, name, value, fallback){
        var setter = "set" + Utils.String.ucfirst(attrName);
        if (typeof obj[setter] == "function") {
            obj[setter].call(obj, value);
        } else if (obj.hasOwnProperty(name)) {
            obj[name] = value;
        } else if (typeof fallback == "function"){
            fallback.call(obj, name, value);
        } else {
            console.warn("No fallback opt on writing object attribute failure.");
        }
    },

    /**
     *
     * @param stackList
     * @param needle
     * @param compareCallback
     * @returns {number}
     */
    calculatePosition: function (stackList, needle, compareCallback){

        if (typeof compareCallback !== "function") {
            compareCallback = function(item1, item2){
                return item1 < item2 ? -1 : (item1 == item2 ? 0 : 1);
            };
        }
        var pos = 0;
        stackList.map(function(item){
            if (item !== needle) {
                var cmpRet = compareCallback.apply(null, [needle, item]);
                if (cmpRet > 0) {
                    pos++;
                }
            }
        });
        return pos;
    },

    /**
     *
     * @param {Function} func
     * @param {number} [threshold]
     * @param {Function} [alt]
     * @returns {Function}
     */
    throttleFunc: function (func, threshold, alt) {
        var last = Date.now();
        threshold = threshold || 100;
        return function () {
            var now = Date.now();
            if (now - last < threshold) {
                if (alt) {
                    alt.apply(this, arguments);
                }
                return;
            }
            last = now;
            func.apply(this, arguments);
        };
    },

    /**
     *
     * @param {Function} func
     * @param {number} [threshold]
     * @param {boolean} [execASAP]
     * @returns {Function}
     */
    debounceFunc: function (func, threshold, execASAP) {
        var timeout = null;
        threshold = threshold || 100;
        return function () {
            var self = this;
            var args = arguments;
            var delayed = function () {
                if (!execASAP) {
                    func.apply(self, args);
                }
                timeout = null;
            };
            if (timeout) {
                clearTimeout(timeout);
            } else if (execASAP) {
                func.apply(self, args);
            }
            timeout = setTimeout(delayed, threshold);
        };
    },

    /**
     *
     * @param name
     * @param callback
     * @param delay
     */
    throttle: function (name, callback, delay){

        if (typeof delay == "undefined") {
            delay = 500; //ms
        } else {
            delay = parseInt(delay);
            if (isNaN(delay) || delay < 0) {
                throw new TypeError("Argument 3 should be a positive integer.");
            }
        }

        if (typeof callback !== "function") {
            throw new TypeError("Argument 2 should be a function, " + typeof(name) + " given.");
        }

        if (typeof name !== "string" && typeof name !== "number") {
            throw new TypeError("Argument 2 should be a String or number, " + typeof(name) + " given.");
        }

        if (!Utils.throttle._timers) {
            Utils.throttle._timers = {};
        } else {
            if (Utils.throttle._timers[name]) {
                clearTimeout(Utils.throttle._timers[name]);
            }
        }

        if (delay === 0) {
            Utils.invokeFunction(callback);
        } else {
            Utils.throttle._timers[name] = setTimeout(function(){
                Utils.invokeFunction(callback);
            }, delay);
        }
    },

    /**
     *
     * @param {object} objects a map
     * @param {function} cmpFunc optional. sort the list with this function.
     * @returns {Array}
     */
    createObjectList: function (objects, cmpFunc){
        var ret = [];
        $.each(objects, function(index, obj){
            ret.push(obj);
        });
        if (typeof cmpFunc == "function") {
            ret.sort(cmpFunc);
        } else {
            ret.sort();
        }
        return ret;
    },

    /**
     *
     * @param e
     * @param label
     */
    dumpError: function (e, label){
        console.group(label ? label : e.toString());
        console.warn(e);
        console.warn(e.stack);
        console.groupEnd();
        window.lastError = e;

        var msg = label ? label + e.toString() : e.toString();
        msg += "\n" + e.stack;
        remoteLogger.logError(msg);
    },

    /**
     *
     * @param number
     * @param allowZero
     * @returns {Number}
     */
    parseNaturalNumber: function (number, allowZero){
        if (typeof number != "number"){
            throw new TypeError(number + " is not of 'number' type. ");
        }
        if (number <= 0.5 && allowZero !== true){
            throw new RangeError(number + " is not a positive number.");
        }
        return parseInt(number);
    },

    /**
     *
     * @param jid
     * @returns {*}
     */
    bare: function (jid){
        if (typeof jid == "string") {
            return jid ? Strophe.getBareJidFromJid(jid).toLowerCase() : "";
        } else {
            return jid;
        }
    },

    /**
     *
     * @param jid1
     * @param jid2
     * @returns {boolean}
     */
    isAtSameDomain: function (jid1, jid2) {
        var domain1 = Utils.bare(jid1).split(/@(conference\.)?/).pop();
        var domain2 = Utils.bare(jid2).split(/@(conference\.)?/).pop();
        return domain1 == domain2;
    },

    /**
     *
     * There is a requirement that we need to create a mixin with various numbers of image.
     * @param {function} callback
     * @param {Array} srcs image src array
     * @param {string} target_el
     * @param {number} size
     */
    create_img_mixin: function (callback, srcs, target_el, size){
        var count = srcs.length;
        if (2 > count || 4 < count ){
            Utils.log("Argument 1 should contain 2 to 4 elements, " + srcs.length + " given.", "warning");
            return false;
        }

        size = size ? size : 42;
        var canvas = document.createElement("canvas"),
            ctx = canvas.getContext("2d");

        canvas.width = size;
        canvas.height = size;
        ctx.fillStyle = "#ffffff";
        ctx.fillRect(0, 0, size, size);

        var locs = [], pendingCount = count;
        switch(count){
            case 2:
                locs = [[0, size/4], [size/2+1, size/4]];
                break;
            case 3:
                locs = [[size/4, 0], [0, size/2+1], [size/2+1, size/2+1]];
                break;
            case 4:
                locs = [ [0, 0], [size/2+1, 0], [0, size/2+1], [size/2+1, size/2+1] ];
                break;
        }
        $.each(srcs, function(k, v){
            var img = new Image();
            img.onload = function(){
                var loc = locs[k];
                ctx.drawImage.call(ctx, img, loc[0], loc[1], size/2-1, size/2-1);
                onDrawComplete(--pendingCount);
            };
            img.src = v;
        });

        function onDrawComplete(pendingCount){
            if (pendingCount === 0) {
                if (target_el) {
                    $(target_el).html(canvas);
                }
                Utils.invokeFunction(callback, null, canvas.toDataURL("image/png"));
            }
        }
    },

    /**
     *
     * @param message
     * @param type
     * @param position
     * @param extraOptions
     * @returns {*}
     */
    notify: function (message, type, position, extraOptions){
        if (typeof extraOptions == "object") {
            extraOptions.text = message;
            extraOptions.layout = position;
            extraOptions.type = type;
            return noty(extraOptions);
        } else {
            return noty({
                "text": message,
                "layout": position,
                "type": type
            });
        }
    },

    /**
     *
     */
    closeAllNotify: function (){
        $.noty.closeAll();
    },

    /**
     *
     * @param obj
     */
    ajaxFileUpload: function (obj) {
        var loading = bui.getParam('resource').loading;
        $("#loading").ajaxStart(function(){
            $(this).attr('src',loading);
            $(this).show();
        }).ajaxComplete(function(){
            $(this).hide();
        });
        try {
            var filesize = obj.files[0].size;
        } catch (e) {  //ie9
            try {
                var img = new Image();
                img.src = obj.value;
                img.style.display='none';
                document.body.appendChild(img);
                setTimeout(function(){
                    document.body.removeChild(img);
                },1000);
                var filesize =img.fileSize;
            } catch (e) {

                $("#errorMsg").html('<div class="alert alert-error">\
    					 <button type="button" class="close" data-dismiss="alert">x</button>\
               		 <span class="errorMessage"></span> \
             	 </div>');

                $(".alert>.errorMessage").text("Please check your file extension. Only jpg, png, gif, jpeg is allowed");
                $("#errorMsg").show();
                return;
            }
        }
        //var filesize = obj.files[0].size;
        if(filesize>2*1024*1024){
            $("#errorMsg").html('<div class="alert alert-error">\
					 <button type="button" class="close" data-dismiss="alert">x</button>\
            <span class="errorMessage"></span> \
          </div>');

            $(".alert>.errorMessage").text(" max size is 2 MB");
            $("#errorMsg").show();
            return;
        }

        Utils.avatarInit();
        Utils.ajaxUploadAvatar();
    },

    /**
     *
     * @returns {boolean}
     */
    ajaxUploadAvatar: function (){
        $.ajaxFileUpload({
            url: bui.url('uploadImageUrl'),
            secureuri:false,
            fileElementId:'img',
            dataType: 'json',
            error:function(){
                //todo
            },
            success: function (json){
                if(json.state==0){
                    $("#errorMsg").html('<div class="alert alert-error">\
                   					 <button type="button" class="close" data-dismiss="alert">x</button>\
                                <span class="errorMessage"></span> \
                              </div>');

                    $(".alert>.errorMessage").text(json.msg);
                    $("#errorMsg").show();
                    return;
                }
                $('#photo>img').attr('src',json.data);
                $("#preview>img").attr('src',json.data);
                Utils.getRealWidthAndShow();
            }
        })

        return false;
    },

    /**
     *
     */
    avatarInit: function (){
        $('#x1').val('');
        $('#y1').val('');
        $('#x2').val('');
        $('#y2').val('');
        $('#w').val('');
        $('#h').val('');
        real_width= 0;
        real_height=0;
        $("#changeAvatar").hide();
        $(".filename").text('No file selected');
        $("#errorMsg").hide();
    },

    /**
     *
     * @param img
     * @param selection
     * @param width_radio
     * @param height_radio
     * @param show_width
     * @param show_height
     */
    selectPreview: function (img, selection,width_radio,height_radio,show_width,show_height){
        if (!selection.width || !selection.height)
            return;
        var scaleX = 100 / (selection.width || 1);
        var scaleY = 100 / (selection.height || 1);
        $('#preview img').css({
            width: Math.round(scaleX * show_width) + 'px',
            height: Math.round(scaleY * show_height) + 'px',
            marginLeft: '-' + Math.round(scaleX * selection.x1) + 'px',
            marginTop: '-' + Math.round(scaleY * selection.y1) + 'px'
        });
        $('#x1').val((selection.x1)/width_radio);
        $('#y1').val((selection.y1)/height_radio);
        $('#x2').val(selection.x2);
        $('#y2').val(selection.y2);

        $('#w').val((selection.width)/width_radio);
        $('#h').val((selection.height)/height_radio);
    },

    /**
     *
     */
    getRealWidthAndShow: function (){
        if(real_width=="0"||real_height=="0"){
            var newImage = new Image();
            var img = $("#photo>img");
            newImage.src = img.attr('src');
            img.load(function() {
                real_width = newImage.width;
                real_height = newImage.height;
                Utils.getRealWidthAndShow();
            });
        }else{
            $("#changeAvatar").show();
            show_width = $("#photo>img").width();
            show_height = $("#photo>img").height();
            var width_radio = show_width/real_width;
            var height_radio = show_height/real_height;
            var ias =  $('#photo>img').imgAreaSelect({
                instance: true,
                aspectRatio: "1:1",
                handles: true,
                x1:0,
                y1:0,
                x2:96,
                y2:96,
                fadeSpeed: 200,
                maxHeight:show_width,
                maxWidth:show_height,
                minHeight:25,
                minWidth:25,
                parent:$("#photo"),
                onInit:function(img,selection){
                    selection.width = parseInt(this.x2-this.x1);
                    selection.height = parseInt(this.y2-this.y1);
                    selection.x1 = this.x1;
                    selection.y1 = this.y1;
                    Utils.selectPreview(img,selection,width_radio,height_radio,show_width,show_height);
                },
                onSelectChange: function (img, selection) {
                    Utils.selectPreview(img,selection,width_radio,height_radio,show_width,show_height);
                }
            });
        }
    }
};
