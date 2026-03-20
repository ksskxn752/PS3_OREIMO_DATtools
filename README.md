学习中，主要为文件分析，实在学不明白，我在尝试修改文本后重新封包进剧情直接白屏<br>
有懂的大佬们能看到的话求求指点一下迷津吧<br>

-----文件结构-----<br>
PS3_GAME/<br>
|- PARAM		#游戏参数信息<br>
|- USRDIR/		#游戏资源核心目录<br>
	|- ENV/		#资源目录1<br>
	|- MOVIE/	#资源目录2<br>
	|- EBOOT.BIN	#主程序<br>
	|- RESOURCE.DAT 资源封装文件<br>
|- TROPDIR/	#奖杯信息<br>
|- LICDIR/		#游戏授权信息<br>
<br>
--RESOURCE.DAT文件分析—<br>
Magic number:  47 50 44 41 36 34 42 59 -> GPDA64BY<br>
offset 0x051C<br>
album.dat									system_boices.dat<br>
character_icon.dat							title.dat<br>
chat.dat										tutorial_texture.dat<br>
cross_fade_test.dat							weekly_result.dat<br>
dlc.dat										effect.dat<br>
event.dat									event_list.dat<br>
extra.dat										free_view_acc_image_big.dat<br>
free_view_image.dat							graduate_data.dat<br>
loading_image.dat							main.dat<br>
model.dat									motion.dat<br>
old_image_event.dat							photo_data.dat<br>
photo_select.dat								pvss.dat<br>
script.dat									script_bg.dat<br>
script_character.dat							script_chat_bg.dat<br>
script_event.dat								script_fade.dat<br>
script_free.dat								script_motion_list.dat<br>
script_window_face.dat						skill.dat<br>
sns.dat										speak.dat<br>
stage.dat									status.dat<br>
status_default_bg.dat							status_frame.dat<br>
<br>
---从文件头开始以8字节分开标记---<br>
47 50 44 41 36 34 42 59 -> magic number -> GPDA64BY<br>
00 00 10 83 00 00 00 00 -> (小端序)文件大小 ->83 10 00 00 -> 2198863872<br>
00 00 00 00 28 00 00 00 -> 文件数量 -> 40<br>
00 10 00 00 00 00 00 00 -> (小端序)目标文件数据的起始位置 -> 0x1000<br>
00 00 00 00 00 00 00 00 -> 留空<br>
8D 5C 01 00 00 00 00 00 -> 寻址标识<br>
18 05 00 00 00 00 00 00 -> (小端序)目标文件名的起始位置 -> 0x518<br>
<br>
文件结构:
Struct Header{
Uint64_t magic #文件头部魔数 8字节
Uint64_t filesize #文件大小 8字节
Uint32_t fileNULL1 #空4字节
Uint16_t filenumber #文件数量 2字节
Uint16_t fileNULL2 #空2字节
}

从offset 0x1000开始以每8字节分开标记
47 50 44 41 36 34 42 59 -> magic number -> GPDA64BY
8D 5C 01 00 00 00 00 00 -> 类似寻址指针
00 00 00 00 02 00 00 00 -> 文件数量 -> 2
00 08 00 00 00 00 00 00 -> (小端序)数据块大小 -> 800 -> 2048 -> 2k对齐
01 00 00 00 00 00 00 00 -> 文件嵌套 -> 1无 0 有
26 35 01 00 00 00 00 00 -> (小端序)文件1大小 -> 13526 -> 79142
58 00 00 00 00 00 00 00 -> (偏移量)文件名1起始位置
00 40 01 00 00 00 00 00 -> (小端序)偏移量 -> 14000 + (当前偏移)1000
01 00 00 00 00 00 00 00 -> 文件嵌套 -> 1无 0 有
8D 1C 00 00 00 00 00 00 -> (小端序)文件2大小 -> 1C8D -> 7639
6D 00 00 00 00 00 00 00 -> (偏移量)文件名2起始位置
往后是文件名长度以及文件名
文件结构:
Struct INFO 
{
  uint64_t filedata_bag #文件起始偏移地址 8字节
  Uint64_t fileNULL #8字节
  Uint64_t offset #文件偏移位置 8字节
  Uint64_t filename_offset #文件名偏移地址 8字节
}
因为没有标明数据块的大小，所有在读数据块的时候是用
filesize =（filedata_bag+1) – filedata_bag
读取最后一个文件时需要先拿到文件大小再减去filedata_bag
Last_filesize = fullfilesize – filedata_bag

--剧情文本—
先是推测scrite、chat、event_list等可能存在message的文件，但几乎都只发现.csv .phyre等文件，其中scrite里有BST_角色名00xx.dat等文件，解开后得到了同文件名的.dat .data.gz .obj.gz等文件
虽说是脚本目录，但也只能推测大概率有文本
现在一层层看，首先解开同目录下的APR_NONE0000.DAT得到了
APR_NONE0000.DAT  APR_NONE0000.DAT.GZ  APR_NONE0000.OBJ.GZ
三个文件
先看APR_NONE0000.DAT 在文件头有GPDA64BY，显然又是资源容器，其中有SE008.HCA SE028.HCA SE012.HCA等文件，可以先跳过这个文件了，看着跟我们需要的文本没什么关系
再看APR_NONE0000.DAT.GZ 跟后缀名gz一样，在文件头有1F8B的gzip标识，解压后拿到.dat的文件，同样是资源容器，但里面有大量.vol文件，每个文件的数据段只有几行二进制 
所以这文件也跟文本没什么关系
剩下最后一个APR_NONE0000.OBJ.GZ  gzip文件，解开后得到.obj文件，.obj这是3D资源文件格式直接打开会报错，那么打开文件，开头就是日期 显然不是什么3D资源文件，但是看着也不像文本，在文件尾发现APR_NONE0000.TXT 不排除这是文本编译出来的产物，先转换编码看看 ，全是NUL等二进制，这也找不到文本，在网上找一下相关信息先
整理一下信息，PS3游戏大多使用的是UTF-8、Shift-JIS编码，在这个文件尝试后也看不到日文的明文，那么尝试一下其他的编码呢，在PS3还会使用UTF-16、EUC-JP等编码，以及自定义的字库编码，做一下最后的尝试，希望不是自定义编码
用notepad++打开这个obj文件，以UTF-16编码显示，但很可惜，也不能看到明文，而notepad++没有EUC-JP编码，既然没有那就换个编辑器做最后的尝试
用Emeditor打开这个obj，从列表中选择编码，在这里发现了UTF-16LE和UTF-16BE两个编码，难道notepad++的编码有误？先尝试用UTF-16LE打开了这个文件 ，没想到发现了明文，感动死我了
