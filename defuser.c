#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

static uint32_t mystep2(uint32_t x, uint32_t y)
{
    uint32_t result = x * UINT32_C(5);
    result += y;
    result ^= UINT32_C(0x4485);
    return result;
}

static uint32_t mystep0(uint32_t x, uint32_t y)
{
    uint32_t result = UINT32_C(5) * x;
    uint32_t temp = y + UINT32_C(0xfb1a);
    result ^= temp;
    return result;
}

static uint32_t mystep1(uint32_t x, uint32_t y)
{
    uint32_t result = x + UINT32_C(4) * y;
    result ^= UINT32_C(0xd533);
    return result;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t middle = (uint32_t)(challenge >> 16);

    uint32_t step2_result = mystep2(high, low);
    uint32_t step0_result = mystep0(low, middle);
    values[0] = mystep1(step0_result, step2_result);

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    (void)challenge;

    uint32_t num=(uint32_t)(challenge>>24);
    num &= UINT32_C(0x3f);
    num += UINT32_C(0xc);
    values[0]=num;
    for(int i=1;i<6;i++)
    {
        uint32_t temp=(uint32_t)(challenge>>(i+7));
        values[i]=(3*values[i-1]-i)^((temp&UINT32_C(0x1f))+2);
    }

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;

    values[0]=(uint32_t)(challenge&7);
    uint32_t temp=(uint32_t)(challenge>>3)^(uint32_t)(challenge>>32);
    switch(values[0])
    {
        case 0:
            values[1] = UINT32_C(5) * temp + UINT32_C(0x5b2c);
            break;
        case 1:
            values[1] = UINT32_C(5) * temp + UINT32_C(0x17188);
            break;
        case 2:
            values[1] = UINT32_C(5) * temp + UINT32_C(0x30ce1);
            break;
        case 3:
            values[1] = UINT32_C(5) * temp + UINT32_C(0x5dc8);
            break;
        case 4:
            values[1] = (UINT32_C(4) * temp) ^ UINT32_C(0xc819);
            break;
        case 5:
            values[1] = (UINT32_C(4) * temp) ^ UINT32_C(0x9e95);
            break;
        case 6:
            values[1] = (temp ^ UINT32_C(0x2a9a)) + (uint32_t)(temp >> 1);
            break;
        case 7:
            values[1] = (temp ^ UINT32_C(0xc884)) + (uint32_t)(temp >> 1);
            break;
    }
    return write_answer(out, cap, values, count);
}

uint32_t p4_walk(uint32_t num1,uint32_t low,uint32_t high,uint32_t num2)
{
    uint32_t mid=(high-low)/UINT32_C(2)+low;
    if(mid==num1) return (num1+UINT32_C(0xc8))^(num2);
    else if(mid<num1)
    {
        uint32_t r=p4_walk(num1,mid+1,high,num2);
        return (UINT32_C(5)*r)^(mid+num2)^(UINT32_C(0x1d));
    }
    else
    {
        uint32_t r=p4_walk(num1,low,mid-1,num2);
        return (UINT32_C(5)*r)^(mid+num2)^(UINT32_C(0x0d));
    }
}
int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;

    uint32_t num1=(uint32_t)(challenge & UINT32_C(0x7f));
    uint32_t num2=(uint32_t)(challenge>>7)^(uint32_t)(challenge>>32);
    values[0]=p4_walk(num1,0,127,num2);
    return write_answer(out, cap, values, count);
}
struct Node
{
    struct Node* next;
    uint32_t value;
    uint32_t tag;
};
static struct Node phase5_nodes[12] =
{
    {&phase5_nodes[11], UINT32_C(320), UINT32_C(41374)},
    {&phase5_nodes[5], UINT32_C(2912), UINT32_C(19273)},
    {&phase5_nodes[9], UINT32_C(2618), UINT32_C(53365)},
    {NULL, UINT32_C(2415), UINT32_C(31311)},
    {&phase5_nodes[8], UINT32_C(1408), UINT32_C(49142)},
    {&phase5_nodes[7], UINT32_C(1842), UINT32_C(27844)},
    {&phase5_nodes[0], UINT32_C(16), UINT32_C(10348)},
    {&phase5_nodes[3], UINT32_C(2086), UINT32_C(63695)},
    {&phase5_nodes[1], UINT32_C(528), UINT32_C(44149)},
    {&phase5_nodes[6], UINT32_C(848), UINT32_C(48945)},
    {&phase5_nodes[2], UINT32_C(1559), UINT32_C(11492)},
    {&phase5_nodes[4], UINT32_C(1066), UINT32_C(47811)},
};

static const struct Node *const phase5_heads[8] =
{
    &phase5_nodes[10], &phase5_nodes[2], &phase5_nodes[9], &phase5_nodes[6],
    &phase5_nodes[0], &phase5_nodes[11], &phase5_nodes[4], &phase5_nodes[8],
};

static uint32_t phase5_traverse(uint64_t challenge, uint32_t *last_value)
{
    uint32_t start = (uint32_t)(challenge & UINT64_C(7));
    uint32_t hops = (uint32_t)((challenge >> 3) & UINT64_C(3));
    const struct Node *node = phase5_heads[start];
    uint32_t result = (uint32_t)(challenge >> 6)
                    ^ (uint32_t)(challenge >> 32)
                    ^ UINT32_C(0x5b81);

    result ^= node->tag;
    result += node->value;
    *last_value = node->value;

    for (uint32_t i = 0; i < hops; ++i)
    {
        node = node->next;
        result ^= node->tag;
        result += node->value;
        *last_value = node->value;
    }

    return result;
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    values[0] = phase5_traverse(challenge, &values[1]);

    return write_answer(out, cap, values, count);
}
static uint32_t my_op5(uint32_t num)
{
    return (num + UINT32_C(0xa687)) ^ (num >> 1);
}

static uint32_t my_op0(uint32_t num)
{
    uint32_t rotated = (num << 1) | (num >> 31);
    return rotated ^ UINT32_C(0x9789);
}

static uint32_t my_op4(uint32_t num)
{
    return (num >> 5) | (num << 27);
}

static uint32_t my_op3(uint32_t num)
{
    return ((num >> 2) ^ num) + UINT32_C(0x9b66);
}

static uint32_t my_op1(uint32_t num)
{
    return (num ^ UINT32_C(0x7f8f)) + UINT32_C(8) * num;
}

static uint32_t my_op2(uint32_t num)
{
    uint32_t scaled = UINT32_C(8) * num;
    uint32_t result = scaled + UINT32_C(0xd10d);
    result ^= scaled - num;
    return result;
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    typedef uint32_t (*Op)(uint32_t);

    static Op const ops[8] =
    {
        my_op5, my_op0, my_op4, my_op3,
        my_op3, my_op1, my_op1, my_op2
    };
    uint32_t first = (uint32_t)(challenge & 7);
    uint32_t second = (uint32_t)((challenge >> 3) & 7);
    uint32_t input = (uint32_t)(challenge >> 6)
                   ^ (uint32_t)(challenge >> 32);

    uint32_t result = ops[first](input);
    result ^= UINT32_C(0xbab2);
    result = ops[second](result);
    values[0] = result + UINT32_C(0xa11d);

    return write_answer(out, cap, values, count);
}

static uint32_t my_secret_mix(uint32_t num)
{
    uint32_t rotated = (num << 3) | (num >> 29);
    return (rotated + UINT32_C(0x1924)) ^ UINT32_C(0x5801);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t first = (uint32_t)context & UINT32_C(3);

    uint32_t challenge_fold = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t context_fold = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t context_term = first * UINT32_C(0x1924);
    context_term ^= (uint32_t)(context >> 16);

    uint32_t second = my_secret_mix(challenge_fold ^ context_fold);
    second ^= context_term;

    uint32_t third_term = context_fold + first + UINT32_C(0x5801);
    third_term += challenge_fold >> 3;
    uint32_t third = second ^ third_term;

    int size = snprintf(out, cap,
                        "AAAAAAAAAAAAAAAA" "AAAAAAAAAAAAAAAA"
                        "OPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
                        first, second, third);
    return size < 0 || (size_t)size >= cap;
}
