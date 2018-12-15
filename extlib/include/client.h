#pragma once
#ifndef XL_ACTIVITY_CLIENT_H_

#define XL_ACTIVITY_CLIENT_VERSION_MAJOR 0
#define XL_ACTIVITY_CLIENT_VERSION_MINOR 14
#define XL_ACTIVITY_CLIENT_VERSION_REVISION 0

#include <cinttypes>
#include <functional>
#include <memory>
#include <set>
#include <map>
#include <string>
#include <vector>

namespace xl {
namespace activity {

// 活动奖励/扣取物品
struct Item {
    using Vec = std::vector<Item>;
    enum Type {
        kTypeCard,      // 房卡
        kTypeGold,      // 金币
        kTypeGiftMoney, // 红包
        kTypeChance,    // 抽奖机会
        kTypeWord,      // 字
        kTypeCustomize  // 自定义道具
    };
    int type; // 奖励类型
    int num;  // 具体数目
    int extra; // 附加数据（用于房卡奖励类型及自定义道具id等）
    uint64_t guid; // GUID
};

// 活动事件定义
struct Event {
    int game;        // 游戏ID
    int activity;    // 活动ID
    int user;        // 玩家ID
    int reward;      // 奖励索引
    Item::Vec items; // 物品列表
};

enum DataType {
    kDataLogin,       // 登录
    kDataInvite,      // 邀请
    kDataShare,       // 分享
    kDataPlay,        // 对局
    kDataPermutation, // 牌型
    kDataCheckIn,     // 签到
    kDataMileStone,   // 里程碑
    kDataCheckBind    // 检测绑定
};

// 基础数据定义
struct Data {
    int game;
    explicit Data(int game) : game(game) {}
    virtual ~Data() {}
    virtual int Type() = 0;
};

template <int TYPE> struct Identifier : public Data {
    explicit Identifier(int game) : Data(game) {}
    int Type() { return TYPE; }
};

// 登录数据
struct Login : public Identifier<kDataLogin> {
    int user;     // 玩家ID
    int previous; // 上一次登录UTC时间戳
    int card;     // 剩余房卡数量
    int regtime;  //注册时间
    Login(int game);
};

// 邀请数据
struct Invite : public Identifier<kDataInvite> {
    int inviter; // 邀请者
    int invitee; // 被邀请者
    Invite(int game);
};

// 分享数据
struct Share : public Identifier<kDataShare> {
    int user; // 玩家ID
    Share(int game);
};

//牌局数据
struct Play : public Identifier<kDataPlay> {
    enum FieldType {
        kGameGeneral, // 普通场
        kGameClub,    // 俱乐部
        kGameGold,    // 金币场
        kGameMatch    // 比赛场
    };
    int user;                   // 玩家ID
    int score;                  // 玩家分数
    int field;                  // 游戏场类型
    int total;                  // 总局数
    int current;                // 当前局数
    std::vector<int> playtypes; // 玩法选项
    Play(int game);
};

// 牌型
struct Permutation : public Identifier<kDataPermutation> {
    int user;                   // 玩家ID
    std::set<int> permutations; // 牌型
    Permutation(int game);
};

// 签到
struct CheckIn : public Identifier<kDataCheckIn> {
    int user; // 玩家ID
    int day;  // 哪天签到
    CheckIn(int game);
};

//里程碑
struct MileStone : public Identifier<kDataMileStone> {
    enum MileStoneType {
        kMileBigWinner, // 大赢家
        kMileBestGunner, // 最佳炮手
        kMileClubMember //俱乐部成员
    };
    int user; // 玩家ID
    int type; // 里程碑类型
    MileStone(int game);
};

// 检测绑定
struct CheckBind : public Identifier<kDataCheckBind> {
    int user;             // 玩家ID
    std::string union_id; //玩家union_id
    CheckBind(int game);
};

class Client {
    struct Impl;

public:
    Client();
    ~Client();

    // 启动 (第一次同步连接成功之后的断连会自动异步重连,无需手动重新Start)
    // host 活动服务器地址
    // port 活动服务端口
    // games 游戏服支持的appid集合
    // error 错误信息
    // handler
    // 奖励回调函数.只在发放房卡、金币的服务器填充.（会被工作线程调用，需注意线程安全）
    // address 服务器http增扣卡地址
    // 返回值表示启动成功和失败
    bool Start(std::string host, int port, std::set<int> games,
               std::function<void(std::string)> error = nullptr,
               std::function<void(Event)> handler = nullptr,
               std::string address = std::string(""));

    // 关闭
    void Stop();

    // 发送活动数据
    void Send(Data *data);

private:
    Client(const Client &);
    const Client &operator=(const Client &);
    std::unique_ptr<Impl> impl_;
};
} // namespace activity
} // namespace xl

#endif // XL_ACTIVITY_CLIENT_H_
