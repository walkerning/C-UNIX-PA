# -*- coding: utf-8 -*-

from __future__ import print_function

from collections import namedtuple

JudgeAnswer = namedtuple("JudgeAnswer", ["quality", "index"])
Strategy = namedtuple("Strategy", ["trials", "judge"])

class MagicScales(object):
    LEFT = 1
    DRAW = 0
    RIGHT = -1

    def __init__(self, trial_num, ball_num):
        self.trail_num = trial_num
        self.ball_num = ball_num
        self.strategy = self.find_strategy(self.trial_num, ball_num)

    @staticmethod
    def find_strategy(trial_num, ball_num):
        """
        Find the trial strategy.

        find_strategy find a strategy for detecting the evil ball
        out of `ball_num` balls within `trial_num` trials.
        """

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
        print("第%d号小球比其他小球%s" % (judge_answer.index, "重" if judge_answer.quality == 1 else "轻"))

    def judge_answer(self, answer):
        return self.strategy.judge[answer]

def main():
    magic_scales = MagicScales(3, 12)
    magic_scales.scale()
