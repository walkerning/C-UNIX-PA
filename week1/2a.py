# -*- coding: utf-8 -*-

class Main(object):
    """
    命题: 若5, 6已经被写出, 20及以上的所有数字都可以被表示.
    证明:
        显然, 存在5之后, 只需要任意一个长度为5的区间的数字能被表示，比这个区间更大的数字都能被表示. 只需要在该区间的数字上 + 5*i即可.
        又, 若5, 6已经被写出, 由于x - 5 + 6 = x + 1, 当x = 4 * 5 = 20的时候，由4个5构成的20这个数字可连续4次减去5加上6，得到20, 21, 22, 23, 24五个数字。命题得证。
    """

    THRESHOLD = 20 # 证明见docstring
    FULL_SET = set(range(1, 20))

    def __init__(self, agent0, agent1):
        self.agent0 = agent0
        self.agent1 = agent1

    def run(self):
        remain_number, exist_number, all_representable = self._init_remain_number()
        last_write = 1
        while remain_number:
            last_write = last_write ^ 1
            print("%s手玩家行动:" % ("先" if last_write == 0 else "后"))
            if last_write == 0:
                write_number = self.agent0.get_action(remain_number, exist_number, all_representable)
            else:
                write_number = self.agent1.get_action(remain_number, exist_number, all_representable)
            print("%s手玩家给出动作: %d\n" % ("先" if last_write == 0 else "后", write_number))
            if write_number == 1:
                print("%s手玩家胜利!\n" % ("后" if last_write == 0 else "先"))
                return
            else:
                try:
                    remain_number, exist_number, all_presentable = self.next_turn(remain_number,
                                                                                  exist_number,
                                                                                  all_representable,
                                                                                  write_number)
                except Exception as e:
                    print(e)
                    print("%s手玩家给出不合法的动作, 请重新动作" % ("先" if last_write == 0 else "后"))
                    last_write = last_write ^ 1
                    continue

    @classmethod
    def next_turn(cls, remain_number, exist_number, all_representable, write_number):
        """
        Parameters
        ----------------
        remain_number: set
            现在剩下的没有表示的number的集合
        exist_number: set
            现在被写在黑板上的number的集合
        all_representable: set
            现在1~19之间能被表达的所有数字的集合
        write_number: int
            这次写下的number

        Returns
        ----------------
        new_remain_number: set
            新的剩下的没有表示的number的集合
        new_exist_number: set
            新的被写在黑板上的number的集合
        new_all_representable: set
            新的1~19之间能被表达的所有数字的集合
        """

        if write_number not in remain_number:
            raise Exception("错误的数字 %d, 该数字可以表示为已有数字 %s 的组合" % (write_number, ", ".join(map(str, exist_number))))

        new_remain_number = remain_number.copy()
        new_remain_number.remove(write_number)
        new_exist_number = exist_number.copy()
        new_exist_number.add(write_number)

        new_number_set = {coeff * write_number + num for num in {0}.union(all_representable) for coeff in range(cls.THRESHOLD/write_number + 1)}
        new_remain_number = new_remain_number.difference(new_number_set)

        new_all_representable = all_representable.union(new_number_set.intersection(cls.FULL_SET))
        return new_remain_number, new_exist_number, new_all_representable

    @classmethod
    def _init_remain_number(cls):
        """
        一开始黑板上有5, 6, 初始化remain_number, exist_number, all_representable"""

        num_set = set()
        for x in range(5):
            for y in range(4):
                num_set.add(x * 5 + y * 6)

        init_remain_number = set()
        for number in range(1, cls.THRESHOLD):
            if number not in num_set:
                init_remain_number.add(number)

        return init_remain_number, {5, 6}, num_set.intersection(cls.FULL_SET)

class HumanAgent(object):
    def get_action(self, remain_number, exist_number, all_representable):
        while True:
            ans = raw_input("剩下的可写数字: {remain_number}\n已写数字: {exist_number}\n现在为止所有可表示的数字: {all_representable}\n".format(
                remain_number=", ".join([str(n) for n in remain_number]),
                exist_number=", ".join([str(n) for n in exist_number]),
                all_representable=", ".join([str(n) for n in all_representable])))
            try:
                ans = int(ans)
            except ValueError:
                continue
            else:
                return ans

class CleverAgent(object):
    def get_action(self, remain_number, exist_number, all_representable):
        action, win = self._get_action(remain_number, exist_number, all_representable)
        if win == 0:
            # 别弃疗... 相信对方是一个愚蠢的人类
            action, win = self._get_action_against_random(remain_number, exist_number, all_representable, True)
        return action

    def _get_action_against_random(self, remain_number, exist_number, all_representable, isself):
        if remain_number == {1}:
            return 1, 0
        if isself:
            min_lose = 1.1
            write_number = 1
            for num in remain_number:
                if num == 1:
                    continue
                new_remain_number, new_exist_number, new_all_representable = Main.next_turn(remain_number, exist_number, all_representable, num)
                _, op_win = self._get_action_against_random(new_remain_number, new_exist_number, new_all_representable, False)
                if op_win < min_lose:
                    write_number = num
                    min_lose = op_win
                    win = 1 - min_lose
            return write_number, win
        else:
            win = 0
            for num in remain_number:
                if num == 1:
                    continue
                new_remain_number, new_exist_number, new_all_representable = Main.next_turn(remain_number, exist_number, all_representable, num)
                _, op_win = self._get_action_against_random(new_remain_number, new_exist_number, new_all_representable, True)
                win += 1 - op_win
            win = float(win) / (num - 1)
            return 1, win

    def _get_action(self, remain_number, exist_number, all_representable):
        if remain_number == {1}:
            return 1, 0
        min_lose = 1.1
        write_number = 1
        for num in remain_number:
            if num == 1:
                continue
            new_remain_number, new_exist_number, new_all_representable = Main.next_turn(remain_number, exist_number, all_representable, num)
            _, op_win = self._get_action(new_remain_number, new_exist_number, new_all_representable)
            if op_win < min_lose:
                write_number = num
                min_lose = op_win
        win = 1 - min_lose
        return write_number, win


if __name__ == "__main__":
    game = Main(CleverAgent(), CleverAgent())
    game.run()
