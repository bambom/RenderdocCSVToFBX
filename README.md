# RenderdocCSVToFBX
## 用途

Renderdoc截帧出来的csv转成fbx。

## 注意事项

- 由于某些lib库是release版，所以只能编译成release的exe在使用。
- Renderdoc截帧完之后直接点模型信息保存和随便点击一行数据在点保存语义不一样，我一般点击一行再行保存。
- fbx库的来源是虚幻扒的。

## 使用流程

将编译出来的exe和bat放置到同一路径下，然后按照hair.bat的样式进行修改即可，有需要自定义的进源码改正。参考examples文件夹下的示例。