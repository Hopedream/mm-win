/**
 * Created by zhiyaoc on 15/3/19.
 */

var AvatarManager = {
	max_size: 2, //MB
	real_width: 0,
	real_height: 0,
	processing: false,
	uploadAvatar: function (obj) {
		if (!this.processing) {
			this.processing = true;

			this.avatarInit();
//			this.showUploading();
			var fileSizeChecked = this.checkFileSize(obj);
			if (fileSizeChecked) {
				this.ajaxUploadAvatar();
			}

			this.processing = false;
		}
	},
	showUploading: function () {
		var loadingIcon = bui.getParam('resource').loading;
		$("#loading").ajaxStart(function () {
			$(this).attr('src', loadingIcon);
			$(this).show();
		}).ajaxComplete(function () {
				$(this).hide();
			});
	},
	showErrorMsg: function (text) {
		$("#errorMsg").html('<div class="alert alert-error">\
    					     <button type="button" class="close" data-dismiss="alert">x</button>\
                             <span class="errorMessage"></span> \
                             </div>');

		$(".alert>.errorMessage").text(text);
		$("#errorMsg").show();
	},
	checkFileSize: function (obj) {
		try {
			var filesize = obj.files[0].size;
		} catch (e) {  //ie9
			try {
				var img = new Image();
				img.src = obj.value;
				img.style.display = 'none';
				document.body.appendChild(img);
				setTimeout(function () {
					document.body.removeChild(img);
				}, 1000);
				var filesize = img.fileSize;
			} catch (e) {
				var errorText = 'Please check your file extension. Only jpg, png, gif, jpeg is allowed';
				this.showErrorMsg(errorText);
				return false;
			}
		}

		if (filesize > this.max_size * 1024 * 1024) {
			var errorText = "max size is " + this.max_size + " MB";
			this.showErrorMsg(errorText);
			return false;
		}

		return true;
	},
	avatarInit: function () {
		$('#x1').val('');
		$('#y1').val('');
		$('#x2').val('');
		$('#y2').val('');
		$('#w').val('');
		$('#h').val('');
		$("#changeAvatar").hide();
		$(".filename").text('No file selected');
		$("#errorMsg").hide();

		this.real_width = 0;
		this.real_height = 0;

	},
	ajaxUploadAvatar: function () {
		var ths = this;
		var csrf = $("#csrf").val();

		$.ajaxFileUpload({
			url:           '/user/upload-image',
			type:          'post',
			secureuri:     false,
			fileElementId: 'avatar_image',
			dataType:      'json',
			data:          {'_csrf': csrf},
			error:         function (XMLHttpRequest, textStatus, error) {
				//todo
				console.log(error);
				ths.showErrorMsg('failed to upload please retry');
			},
			success:       function (json) {
				if (json.state == 0) {
					ths.showErrorMsg(json.msg);
				} else {
					$('#photo>img').attr('src', json.data);
					$("#preview>img").attr('src', json.data);
					ths.onFileUploaded();
				}

			}
		})
	},
	getImagePixel:       function () {
		var ths = this;
		if (ths.real_width == "0" || ths.real_height == "0") {
			var newImage = new Image();
			var img = $("#photo>img");
			newImage.src = img.attr('src');
			img.load(function () {
				ths.real_width = newImage.width;
				ths.real_height = newImage.height;
				ths.getImagePixel();
			});
		} else {
			ths.displayImage();
		}
	},
	displayImage: function () {
		var ths = this;
		var isIE = navigator.userAgent.indexOf("MSIE") >= 0;

		$("#changeAvatar").show();
		$('#selected-area').show();
		var show_width = $("#photo>img").width();
		var show_height = $("#photo>img").height();
		var width_radio = show_width / this.real_width;
		var height_radio = show_height / this.real_height;
		var ias = $('#photo>img').imgAreaSelect({
			instance: true,
			aspectRatio: "1:1",
			handles: true,
			x1: 0,
			y1: 0,
			x2: 96,
			y2: 96,
			fadeSpeed: 200,
			maxHeight: show_width,
			maxWidth: show_height,
			minHeight: 25,
			minWidth: 25,
			parent: isIE ? $('#changeAvatar') : $("#selected-area"),
			onInit: function (img, selection) {
				selection.width = parseInt(this.x2 - this.x1);
				selection.height = parseInt(this.y2 - this.y1);
				selection.x1 = this.x1;
				selection.y1 = this.y1;
				ths.previewSelectedArea(img, selection, width_radio, height_radio, show_width, show_height)
			},
			onSelectChange: function (img, selection) {
				ths.previewSelectedArea(img, selection, width_radio, height_radio, show_width, show_height)
			}
		});
	},
	onFileUploaded: function () {
		if (this.real_width == "0" || this.real_height == "0") {
			this.getImagePixel();
		} else {
			this.displayImage();
		}
	},
	previewSelectedArea: function (img, selection, width_radio, height_radio, show_width, show_height) {
		if (!selection.width || !selection.height)
			return;
		var scaleX = 100 / (selection.width || 1);
		var scaleY = 100 / (selection.height || 1);
		$('#preview img').css({
			width: Math.round(scaleX * show_width) + 'px',
			height: Math.round(scaleY * show_height) + 'px',
			marginLeft: '-' + Math.round(scaleX * selection.x1) + 'px',
			marginTop:  '-' + Math.round(scaleY * selection.y1) + 'px'
		});
		$('#x1').val((selection.x1) / width_radio);
		$('#y1').val((selection.y1) / height_radio);
		$('#x2').val(selection.x2);
		$('#y2').val(selection.y2);

		$('#w').val((selection.width) / width_radio);
		$('#h').val((selection.height) / height_radio);
	},

	cropImage: function () {
		var x1 = $('#x1').val();
		var y1 = $('#y1').val();
		var x2 = $('#x2').val();
		var y2 = $('#y2').val();
		var w = $('#w').val();
		var h = $('#h').val();
		var pic = $('#photo').find('>img').attr('src');
		var ths = this;
		if (pic == "") {
			this.showErrorMsg("please choose a image to upload")
		} else {
			$.post("/user/change-avatar", {x: x1, y: y1, w: w, h: h, pic: pic }, function (json) {
				if (json.state == 1) {
					$(".nav-user-avatar").attr("src", json.url);
					$(".change-avatar").attr("src", json.url);
				} else {
					ths.showErrorMsg(json.msg);
				}

				$('#photo').find('>img').imgAreaSelect({
					remove: true
				});

				$('#user-avatar-change').modal('hide');

			}, 'json');
		}
	}
};