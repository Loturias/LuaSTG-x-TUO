# 分支管理

本仓库采用以下分支模型。

## 分支职责

| 分支 | 用途 | 规则 |
| --- | --- | --- |
| `master` | 原仓库镜像 | 只接受上游 fast-forward；保持与上游一致，禁止提交自有修改 |
| `develop` | 下一版本集成 | 接收完成的功能和上游更新；发布前统一验证 |
| `stable` | 稳定发行版 | 只接收已完整编译测试的 `develop` 或紧急修复；正式版本在此打标签 |
| `feature/*` | 自有功能 | 从 `develop` 建立；一个分支只处理一个功能，完成后合入 `develop` 并删除 |
| `contrib/*` | 上游 PR | 从 `master` 建立；一个 PR 使用一个独立分支，不依赖私有功能 |
| `hotfix/*` | 已发布版本修复 | 从 `stable` 建立；修复后同时合入 `stable` 和 `develop` |

不得建立永久统一的 PR 分支。不同上游贡献必须使用独立的 `contrib/*`。

允许的主要流向：

```text
upstream/master -> master -> develop -> stable -> version tag
                              ^
feature/* --------------------+

master -> contrib/* -> upstream PR
stable -> hotfix/* -> stable + develop
```

禁止将 `develop`、`stable` 或 `feature/*` 合入 `master`。上游更新先同步到 `master`，再合入 `develop` 解决冲突和验证。

## Git 操作边界

- 使用 `merge` 保留 `master -> develop`、`develop -> stable` 等长期关系。
- `rebase` 只用于尚未共享的个人 `feature/*` 或 `contrib/*`，不得重写共享长期分支。
- 禁止对 `master`、`stable` 和 `develop` force push。
- 提交保持单一职责，说明修改原因；不要把格式化、依赖升级和功能修改混在一起。

## 子模块规则

父仓库记录子模块提交 ID，而不是分支最新状态。被父仓库引用的提交必须已经推送到 `.gitmodules` 指向的可访问远程，禁止记录只存在于本机的提交。
修改嵌套子模块时，必须由内向外逐层提交和推送 gitlink，最后提交顶层仓库。切换分支、合并或发布前，主仓库和所有子模块都必须干净。

## Agent 开始任务前检查

开始任务前必须检查当前分支、主仓库工作区和递归子模块状态，并根据任务选择正确分支。不得强制切换到固定开发分支。发现任何未提交修改或未跟踪文件时必须停止，不得自行丢弃、覆盖、暂存或隐藏。
