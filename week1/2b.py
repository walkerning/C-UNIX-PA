# -*- coding: utf-8 -*-

from __future__ import print_function

from collections import namedtuple

JudgeAnswer = namedtuple("JudgeAnswer", ["quality", "index"])
Strategy = namedtuple("Strategy", ["trials", "judge"])

class MagicScales(object):
    LEFT = 1
    DRAW = 0
    RIGHT = -1

    def __init__(self):
        self.strategy = self.find_strategy()

    @staticmethod
    def find_strategy():
        """
        Prepare the trial strategy.
        """
        trials = [[1, 1, 1, 1, -1, -1, -1, -1, 0, 0, 0, 0],
                  [1, -1, -1, 0, 1, -1, 0, 0, 1, -1, 1, 0],
                  [-1, -1, 0, 1, -1, 0, 0, 1, 1, 1, 0, -1]]
        judge = {}
        for index, error_vector in enumerate(zip(*trials)):
            judge[tuple(error_vector)] = JudgeAnswer(index=index, quality=1)
            judge[tuple([-x for x in error_vector])] = JudgeAnswer(index=index, quality=-1)
        return Strategy(trials=trials, judge=judge)

    def scale(self):
        """
        Interact with user and print out the judgement.
        """
        print("输入1代表左边重, 输入0代表一样重, 输入-1代表右边重")
        answer_list = []
        for trial in self.strategy.trials:
            answer = None
            while answer is None:
                answer = raw_input("天平左边放置了: {left}.\t\t天平右边放置了: {right}.\n哪边重呢(1/0/-1)? ".format(
                    left=", ".join([str(index) for index, pos in enumerate(trial) if pos == MagicScales.LEFT]),
                    right=", ".join([str(index) for index, pos in enumerate(trial) if pos == MagicScales.RIGHT])
                ))
                try:
                    answer = int(answer)
                except ValueError:
                    continue
                if answer not in {MagicScales.LEFT, MagicScales.DRAW, MagicScales.RIGHT}:
                    answer = None
                    continue
                answer_list.append(answer)

        judge_answer = self.judge_answer(tuple(answer_list))
        if judge_answer is None:
            print("用户输入不太对\n")
            return (-1, 0)
        else:
            print("第%d号小球比其他小球%s\n" % (judge_answer.index, "重" if judge_answer.quality == 1 else "轻"))
            return (judge_answer.index, judge_answer.quality)

    def judge_answer(self, answer):
        return self.strategy.judge.get(answer, None)

def main():
    magic_scales = MagicScales()
    while True:
        magic_scales.scale()

if __name__ == "__main__":
    main()
