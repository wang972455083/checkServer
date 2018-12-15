/*
Navicat MySQL Data Transfer

Source Server         : 本机
Source Server Version : 50520
Source Host           : localhost:3306
Source Database       : logic

Target Server Type    : MYSQL
Target Server Version : 50520
File Encoding         : 65001

Date: 2017-04-05 15:49:57
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `activelog`
-- ----------------------------
DROP TABLE IF EXISTS `activelog`;
CREATE TABLE `activelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `activeID` int(11) NOT NULL,
  `userID` int(11) NOT NULL,
  `phone` varchar(20) NOT NULL DEFAULT '',
  `reward` int(11) NOT NULL,
  `date` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activelog
-- ----------------------------

-- ----------------------------
-- Table structure for `log`
-- ----------------------------
DROP TABLE IF EXISTS `log`;
CREATE TABLE `log` (
  `Id` varchar(64) NOT NULL,
  `Time` int(11) DEFAULT '0',
  `Pos1` int(11) DEFAULT '0',
  `Pos2` int(11) DEFAULT '0',
  `Pos3` int(11) DEFAULT '0',
  `Pos4` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `DeskId` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `Pass` varchar(50) DEFAULT '',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Reset` int(11) DEFAULT '0',
  `Data` varchar(2048) DEFAULT '',
  `PlayType` varchar(64) DEFAULT '',
  `serverID` int(11) DEFAULT '33001',
  PRIMARY KEY (`Id`),
  KEY `Pos1` (`Pos1`),
  KEY `Pos2` (`Pos2`),
  KEY `Time` (`Time`),
  KEY `Pos3` (`Pos3`),
  KEY `Pos4` (`Pos4`),
  KEY `MaxCircle` (`MaxCircle`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for `sharevideo`
-- ----------------------------
DROP TABLE IF EXISTS `sharevideo`;
CREATE TABLE `sharevideo` (
  `VideoId` varchar(64) NOT NULL,
  `ShareId` int(11) NOT NULL DEFAULT '0',
  `Time` int(11) NOT NULL DEFAULT '0',
  `UserId1` int(11) NOT NULL DEFAULT '0',
  `UserId2` int(11) NOT NULL DEFAULT '0',
  `UserId3` int(11) NOT NULL DEFAULT '0',
  `UserId4` int(11) NOT NULL DEFAULT '0',
  `Score1` int(11) NOT NULL DEFAULT '0',
  `Score2` int(11) NOT NULL DEFAULT '0',
  `Score3` int(11) NOT NULL DEFAULT '0',
  `Score4` int(11) NOT NULL DEFAULT '0',
  `Zhuang` int(11) NOT NULL DEFAULT '0',
  `Data` varchar(4096) NOT NULL,
  `PlayType` varchar(64) NOT NULL,
  `DeskId` int(11) NOT NULL,
  `Flag` int(11) NOT NULL DEFAULT '0',
  `MaxCircle` int(11) NOT NULL DEFAULT '0',
  `CurCircle` int(11) NOT NULL DEFAULT '0',
  `serverID` int(11) DEFAULT '33001',
  `resultData` varchar(4096) NOT NULL DEFAULT '0',
  PRIMARY KEY (`VideoId`),
  KEY `ShareId` (`ShareId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of sharevideo
-- ----------------------------

-- ----------------------------
-- Table structure for `useractive`
-- ----------------------------
DROP TABLE IF EXISTS `useractive`;
CREATE TABLE `useractive` (
  `DateTime` int(11) NOT NULL,
  `NewUser` int(11) DEFAULT '0',
  `ActiveUser` int(11) DEFAULT '0',
  `PlayCount` int(11) DEFAULT '0',
  PRIMARY KEY (`DateTime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of useractive
-- ----------------------------

-- ----------------------------
-- Table structure for `video`
-- ----------------------------
DROP TABLE IF EXISTS `video`;
CREATE TABLE `video` (
  `Id` varchar(64) NOT NULL,
  `UserId1` int(11) DEFAULT NULL,
  `Time` int(11) DEFAULT '0',
  `UserId2` int(11) DEFAULT NULL,
  `UserId3` int(11) DEFAULT NULL,
  `UserId4` int(11) DEFAULT NULL,
  `Data` varchar(4096) DEFAULT NULL,
  `DeskId` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Zhuang` int(11) DEFAULT '0',
  `PlayType` varchar(64) DEFAULT '',
  `serverID` int(11) DEFAULT '33001',
  `resultData` varchar(4096) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`),
  KEY `Time` (`Time`),
  KEY `UserId1` (`UserId1`),
  KEY `UserId2` (`UserId2`),
  KEY `UserId3` (`UserId3`),
  KEY `UserId4` (`UserId4`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

----------------------------
--大转盘抽奖
----------------------------
DROP TABLE IF EXISTS `activeGiftlog`;
CREATE TABLE `activeGiftlog` (
      `Id` int(11)  UNSIGNED NOT NULL AUTO_INCREMENT COMMENT ' 自增主键ID ',
      `ActiveId` int(11) NOT NULL COMMENT ' 活动类型 ',
      `UserId` int(11) NOT NULL DEFAULT '0' COMMENT ' 玩家id ',
      `SpendType` int(11) NOT NULL DEFAULT '0' COMMENT ' 消耗的类型(房卡或免费) ',
      `SpendCount` int(11) NOT NULL DEFAULT '0' COMMENT ' 消耗的数量 ',
      `GiftName` varchar(20) NOT NULL DEFAULT '' COMMENT ' 奖品名称 ',
      `GiftType` int(11) NOT NULL DEFAULT '0' COMMENT ' 奖品类型 ',
      `GiftCount` int(11) NOT NULL DEFAULT '0' COMMENT ' 奖品数量 ',
      `Date` int(11) NOT NULL DEFAULT '0' COMMENT ' 抽奖日期 ',
      `Phone` varchar(20) NOT NULL DEFAULT '' COMMENT ' 领奖号码 ',
      PRIMARY KEY (`Id`),
      KEY `UserId` (`UserId`),
      KEY `Date` (`Date`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT=' 大转盘中奖纪录 ';
	
-----------------------------
--比赛场玩家积分榜
-----------------------------
DROP TABLE IF EXISTS `matchscore`;
 CREATE TABLE `matchscore` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `matchId` int(11) NOT NULL COMMENT ' 比赛场ID ',
  `userInfo` varchar(4096) COLLATE utf8mb4_bin NOT NULL COMMENT ' 参赛人员名次信息 ',
  `matchTime` int(11) NOT NULL DEFAULT '0' COMMENT ' 比赛时间戳 ',
  `matchManager` int(11) NOT NULL  COMMENT ' 比赛场创建人id ',
  PRIMARY KEY (`Id`),
  UNIQUE KEY `matchId` (`matchId`),
  KEY `gameStarter` (`matchManager`) USING BTREE
  ) ENGINE=InnoDB  DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;
  
  --------------------------
  --比赛场创建权限
  --------------------------
  DROP TABLE IF EXISTS `matchmanagers`;
   CREATE TABLE `matchmanagers` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增id ',
  `userId` int(11) NOT NULL DEFAULT '0' COMMENT ' 可开启比赛场的userid ',
  `status` int(11) NOT NULL DEFAULT '0' COMMENT ' 当前用户权限状态，0为正常 ',
  PRIMARY KEY (`id`),
  UNIQUE KEY `managerId` (`userId`) USING BTREE
  ) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;
  
  -------------------------
  --红包活动绑定关系表
  -------------------------
  DROP TABLE IF EXISTS `activitybindingrelation`;
  CREATE TABLE `activitybindingrelation` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT ' 自增ID ',
  `invitee` int(11) NOT NULL DEFAULT '0' COMMENT ' 被邀请者 ',
  `inviter` int(11) NOT NULL DEFAULT '0' COMMENT ' 邀请者 ',
  `bindTime` int(11) NOT NULL DEFAULT '0' COMMENT ' 绑定时间戳 ',
  `firstTaskFinish` int(11) NOT NULL DEFAULT '0' COMMENT ' 绑定后打的局数(大于0认为任务完成) ',
  `secondTaskFinish` int(11) NOT NULL DEFAULT '0' COMMENT ' 绑定后第二天打的局数(大于0认为任务完成) ',
  PRIMARY KEY (`id`),
  UNIQUE KEY `invitee` (`invitee`) USING BTREE,
  KEY `inviter` (`inviter`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT=' 红包活动绑定关系表 ';

-------------------------
--领取红包记录表
-------------------------
  DROP TABLE IF EXISTS `redpacketactivitylog`;
CREATE TABLE `redpacketactivitylog` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT ' 自增ID ',
  `SpecialId` int(11) NOT NULL COMMENT ' 红包特殊ID ',
  `UserId` int(11) NOT NULL DEFAULT '0' COMMENT ' 玩家id ',
  `GiftType` int(11) NOT NULL DEFAULT '0' COMMENT ' 奖品类型 ',
  `GiftCount` int(11) NOT NULL DEFAULT '0' COMMENT ' 奖品个数 ',
  `Date` int(11) NOT NULL DEFAULT '0' COMMENT ' 日期 ',
  PRIMARY KEY (`id`),
  KEY `SpecialId` (`SpecialId`),
  KEY `UserId` (`UserId`),
  KEY `Date` (`Date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT=' 领取红包记录表 ';

------------------------------------------------
--新转盘活动
------------------------------------------------
CREATE TABLE `NewZhuanPanActivityInfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `UserId` int(11) NOT NULL COMMENT '玩家id',
  `DailyMaxWinnerCnt` int(11) NOT NULL DEFAULT '0' COMMENT '记录玩家每天最大赢家次数',
  `DailyMaxWinnerDarwCnt` int(11) NOT NULL DEFAULT '0' COMMENT '记录最大赢家获得的抽奖次数',
  `DailyCreateRoomCnt` int(11) NOT NULL DEFAULT '0' COMMENT '记录玩家创建房间的次数',
  `DailyCreateRoomDarwCnt` int(11) NOT NULL DEFAULT '0' COMMENT '记录创建房间获得的抽奖次数',
  `LastFenXiangTime` int(11) NOT NULL DEFAULT '0' COMMENT '记录上次分享的时间',
  `ContinueFenXiangDays` int(11) NOT NULL DEFAULT '0' COMMENT '记录连续分享的天数',
  `ResetTime` int(11) NOT NULL DEFAULT '0' COMMENT '数据重置时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `UserId` (`UserId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--------------------------------------------
--人满晋级赛
--------------------------------------------
CREATE TABLE `renmanmatchrecord` (
  `Id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增Id',
  `userId` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `matchRoomIndex` int(11) NOT NULL DEFAULT '0' COMMENT '比赛房间索引',
  `matchName` varchar(20) DEFAULT '' COMMENT '比赛名称',
  `matchTime` int(11) NOT NULL DEFAULT '0' COMMENT '比赛时间',
  `mc` smallint(6) NOT NULL DEFAULT '0' COMMENT '名次',
  `matchCardLevel` tinyint(4) NOT NULL DEFAULT '0' COMMENT '比赛资格券等级',
  `rewardType` tinyint(4) NOT NULL DEFAULT '0' COMMENT '奖品类型',
  `rewardCount` smallint(6) NOT NULL DEFAULT '0' COMMENT '奖品数量',
  `lastTurn` tinyint(4) NOT NULL DEFAULT '0' COMMENT '最后一轮标识',
  `playState` int(11) NOT NULL DEFAULT '0' COMMENT '玩法ID',
  `athletesNum` int(11) NOT NULL DEFAULT '0' COMMENT '参赛人数',
  `promotionNum` int(11) NOT NULL DEFAULT '0' COMMENT '晋级人数',
  `taoTai` tinyint(4) NOT NULL DEFAULT '0' COMMENT '淘汰标识',
  PRIMARY KEY (`Id`),
  KEY `userId` (`userId`),
  KEY `matchTime` (`matchTime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

------------------------------------------
--转盘红包活动
------------------------------------------
CREATE TABLE `zhuanpanredpacket` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `userId` int(11) NOT NULL DEFAULT '0' COMMENT '玩家id',
  `drawTime` int(11) NOT NULL DEFAULT '0' COMMENT '抽奖的时间',
  `drawCount` int(11) NOT NULL DEFAULT '0' COMMENT '抽奖的次数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `userId` (`userId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='新转盘红包活动';